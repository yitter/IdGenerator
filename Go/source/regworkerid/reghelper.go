package regworkerid

import (
	"context"
	"fmt"
	"github.com/go-redis/redis/v8"
	"strconv"
	"sync"
	"time"
)

var _client *redis.Client
var _ctx = context.Background()
var _workerIdLock sync.Mutex

var _workerIdList []int32 // 当前已注册的WorkerId
var _loopCount = 0        // 循环数量
var _lifeIndex = -1       // WorkerId本地生命时序（本地多次注册时，生命时序会不同）
var _token = -1           // WorkerId远程注册时用的token，将存储在 IdGen:WorkerId:Value:xx 的值中（本功能暂未启用）

var _WorkerIdLifeTimeSeconds = 15    // IdGen:WorkerId:Value:xx 的值在 redis 中的有效期（单位秒，最好是3的整数倍）
var _MaxLoopCount = 10               // 最大循环次数（无可用WorkerId时循环查找）
var _SleepMillisecondEveryLoop = 200 // 每次循环后，暂停时间
var _MaxWorkerId int32 = 0           // 最大WorkerId值，超过此值从0开始

var _RedisConnString = ""
var _RedisPassword = ""

const _WorkerIdIndexKey string = "IdGen:WorkerId:Index"        // redis 中的key
const _WorkerIdValueKeyPrefix string = "IdGen:WorkerId:Value:" // redis 中的key
const _WorkerIdFlag = "Y"                                      // IdGen:WorkerId:Value:xx 的值（将来可用 _token 替代）
const _Log = false                                              // 是否输出日志

func Validate(workerId int32) int32 {
	for _, value := range _workerIdList {
		if value == workerId {
			return 1
		}
	}

	return 0

	//if workerId == _usingWorkerId {
	//	return 0
	//} else {
	//	return -1
	//}
}

func UnRegister() {
	_workerIdLock.Lock()

	_lifeIndex = -1
	for _, value := range _workerIdList {
		if value > -1 {
			_client.Del(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(int(value)))
		}
	}
	_workerIdList = []int32{}

	_workerIdLock.Unlock()
}

func autoUnRegister() {
	// 如果当前已注册过 WorkerId，则先注销，并终止先前的自动续期线程
	if len(_workerIdList) > 0 {
		UnRegister()
	}
}

func RegisterMany(ip string, port int32, password string, maxWorkerId int32, totalCount int32) []int32 {
	if maxWorkerId < 0 {
		return []int32{-2}
	}

	if totalCount < 1 {
		return []int32{-1}
	}

	autoUnRegister()

	_MaxWorkerId = maxWorkerId
	_RedisConnString = ip + ":" + strconv.Itoa(int(port))
	_RedisPassword = password
	_client = newRedisClient()
	if _client == nil {
		return []int32{-1}
	}
	defer func() {
		if _client != nil {
			_ = _client.Close()
		}
	}()
	//_, err := _client.Ping(_ctx).Result()
	//if err != nil {
	//	//panic("init redis error")
	//	return []int{-3}
	//} else {
	//	if _Log {
	//		fmt.Println("init redis ok")
	//	}
	//}

	_lifeIndex++
	_workerIdList = make([]int32, totalCount)
	for key := range _workerIdList {
		_workerIdList[key] = -1 // 全部初始化-1
	}

	useExtendFunc := false
	for key := range _workerIdList {
		id := register(_lifeIndex)
		if id > -1 {
			useExtendFunc = true
			_workerIdList[key] = id //= append(_workerIdList, id)
		} else {
			break
		}
	}

	if useExtendFunc {
		go extendLifeTime(_lifeIndex)
	}

	return _workerIdList
}

func RegisterOne(ip string, port int32, password string, maxWorkerId int32) int32 {
	if maxWorkerId < 0 {
		return -2
	}

	autoUnRegister()

	_MaxWorkerId = maxWorkerId
	_RedisConnString = ip + ":" + strconv.Itoa(int(port))
	_RedisPassword = password
	_loopCount = 0
	_client = newRedisClient()
	if _client == nil {
		return -3
	}
	defer func() {
		if _client != nil {
			_ = _client.Close()
		}
	}()
	//_, err := _client.Ping(_ctx).Result()
	//if err != nil {
	//	// panic("init redis error")
	//	return -3
	//} else {
	//	if _Log {
	//		fmt.Println("init redis ok")
	//	}
	//}

	_lifeIndex++
	var id = register(_lifeIndex)
	if id > -1 {
		_workerIdList = []int32{id}
		go extendLifeTime(_lifeIndex)
	}

	return id
}

func register(lifeTime int) int32 {
	_loopCount = 0
	return getNextWorkerId(lifeTime)
}

func newRedisClient() *redis.Client {
	return redis.NewClient(&redis.Options{
		Addr:     _RedisConnString,
		Password: _RedisPassword,
		DB:       0,
		//PoolSize:     1000,
		//ReadTimeout:  time.Millisecond * time.Duration(100),
		//WriteTimeout: time.Millisecond * time.Duration(100),
		//IdleTimeout:  time.Second * time.Duration(60),
	})
}

func getNextWorkerId(lifeTime int) int32 {
	// 获取当前 WorkerIdIndex
	r, err := _client.Incr(_ctx, _WorkerIdIndexKey).Result()
	if err != nil {
		return -1
	}

	candidateId := int32(r)
	if _Log {
		fmt.Println("Begin candidateId:" + strconv.Itoa(int(candidateId)))
	}

	// 如果 candidateId 大于最大值，则重置
	if candidateId > _MaxWorkerId {
		if canReset() {
			// 当前应用获得重置 WorkerIdIndex 的权限
			setWorkerIdIndex(-1)
			endReset() // 此步有可能不被执行？
			_loopCount++

			// 超过一定次数，直接终止操作
			if _loopCount > _MaxLoopCount {
				_loopCount = 0

				// 返回错误
				return -1
			}

			// 每次一个大循环后，暂停一些时间
			time.Sleep(time.Duration(_SleepMillisecondEveryLoop*_loopCount) * time.Millisecond)

			if _Log {
				fmt.Println("canReset loop")
			}

			return getNextWorkerId(lifeTime)
		} else {
			// 如果有其它应用正在编辑，则本应用暂停200ms后，再继续
			time.Sleep(time.Duration(200) * time.Millisecond)

			if _Log {
				fmt.Println("not canReset loop")
			}

			return getNextWorkerId(lifeTime)
		}
	}

	if _Log {
		fmt.Println("candidateId:" + strconv.Itoa(int(candidateId)))
	}

	if isAvailable(candidateId) {
		if _Log {
			fmt.Println("AA: isAvailable:" + strconv.Itoa(int(candidateId)))
		}

		// 最新获得的 WorkerIdIndex，在 redis 中是可用状态
		setWorkerIdFlag(candidateId)
		_loopCount = 0

		// 获取到可用 WorkerId 后，启用新线程，每隔 1/3个 _WorkerIdLifeTimeSeconds 时间，向服务器续期（延长一次 LifeTime）
		// go extendWorkerIdLifeTime(lifeTime, candidateId)

		return candidateId
	} else {
		if _Log {
			fmt.Println("BB: not isAvailable:" + strconv.Itoa(int(candidateId)))
		}
		// 最新获得的 WorkerIdIndex，在 redis 中是不可用状态，则继续下一个 WorkerIdIndex
		return getNextWorkerId(lifeTime)
	}
}

func extendLifeTime(lifeIndex int) {
	// 获取到可用 WorkerId 后，启用新线程，每隔 1/3个 _WorkerIdLifeTimeSeconds 时间，向服务器续期（延长一次 LifeTime）
	var myLifeIndex = lifeIndex

	// 循环操作：间隔一定时间，刷新 WorkerId 在 redis 中的有效时间。
	for {
		time.Sleep(time.Duration(_WorkerIdLifeTimeSeconds/3) * time.Second)

		// 上锁操作，防止跟 UnRegister 操作重叠
		_workerIdLock.Lock()

		// 如果临时变量 myLifeIndex 不等于 全局变量 _lifeIndex，表明全局状态被修改，当前线程可终止，不应继续操作 redis
		if myLifeIndex != _lifeIndex {
			break
		}

		// 已经被注销，则终止（此步是上一步的二次验证）
		if len(_workerIdList) < 1 {
			break
		}

		// 延长 redis 数据有效期
		for _, value := range _workerIdList {
			if value > -1 {
				extendWorkerIdFlag(value)
			}
		}

		_workerIdLock.Unlock()
	}
}

func extendWorkerIdLifeTime(lifeIndex int, workerId int32) {
	var myLifeIndex = lifeIndex
	var myWorkerId = workerId

	// 循环操作：间隔一定时间，刷新 WorkerId 在 redis 中的有效时间。
	for {
		time.Sleep(time.Duration(_WorkerIdLifeTimeSeconds/3) * time.Second)

		// 上锁操作，防止跟 UnRegister 操作重叠
		_workerIdLock.Lock()

		// 如果临时变量 myLifeIndex 不等于 全局变量 _lifeIndex，表明全局状态被修改，当前线程可终止，不应继续操作 redis
		if myLifeIndex != _lifeIndex {
			break
		}

		// 已经被注销，则终止（此步是上一步的二次验证）
		//if _usingWorkerId < 0 {
		//	break
		//}

		// 延长 redis 数据有效期
		extendWorkerIdFlag(myWorkerId)

		_workerIdLock.Unlock()
	}
}

func get(key string) (string, bool) {
	r, err := _client.Get(_ctx, key).Result()
	if err != nil {
		return "", false
	}
	return r, true
}

func set(key string, val string, expTime int32) {
	_client.Set(_ctx, key, val, time.Duration(expTime)*time.Second)
}

func setWorkerIdIndex(val int) {
	_client.Set(_ctx, _WorkerIdIndexKey, val, 0)
}

func setWorkerIdFlag(workerId int32) {
	_client.Set(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(int(workerId)), _WorkerIdFlag, time.Duration(_WorkerIdLifeTimeSeconds)*time.Second)
}

func extendWorkerIdFlag(workerId int32) {
	var client = newRedisClient()
	if client == nil {
		return
	}
	defer func() {
		if client != nil {
			_ = client.Close()
		}
	}()

	client.Expire(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(int(workerId)), time.Duration(_WorkerIdLifeTimeSeconds)*time.Second)
}

func canReset() bool {
	r, err := _client.Incr(_ctx, _WorkerIdValueKeyPrefix+"Edit").Result()
	if err != nil {
		return false
	}

	if _Log {
		fmt.Println("canReset:" + strconv.Itoa(int(r)))
	}

	return r != 1
}

func endReset() {
	// _client.Set(_WorkerIdValueKeyPrefix+"Edit", 0, time.Duration(2)*time.Second)
	_client.Set(_ctx, _WorkerIdValueKeyPrefix+"Edit", 0, 0)
}

func getWorkerIdFlag(workerId int32) (string, bool) {
	r, err := _client.Get(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(int(workerId))).Result()
	if err != nil {
		return "", false
	}
	return r, true
}

func isAvailable(workerId int32) bool {
	r, err := _client.Get(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(int(workerId))).Result()

	if _Log {
		fmt.Println("XX isAvailable:" + r)
		fmt.Println("YY isAvailable:" + err.Error())
	}

	if err != nil {
		if err.Error() == "redis: nil" {
			return true
		}
		return false
	}

	return r != _WorkerIdFlag
}
