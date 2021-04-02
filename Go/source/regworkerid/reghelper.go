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

var _usingWorkerId int = -1 // 当前已注册的WorkerId
var _loopCount int = 0      // 循环数量
var _lifeIndex int = -1     // WorkerId本地生命时序（本地多次注册时，生命时序会不同）
var _token int = -1         // WorkerId远程注册时用的token，将存储在 IdGen:WorkerId:Value:xx 的值中（本功能暂未启用）

var _WorkerIdLifeTimeSeconds = 15    // IdGen:WorkerId:Value:xx 的值在 redis 中的有效期（单位秒，最好是3的整数倍）
var _MaxLoopCount = 10               // 最大循环次数（无可用WorkerId时循环查找）
var _SleepMillisecondEveryLoop = 200 // 每次循环后，暂停时间
var _MaxWorkerId int = 0             // 最大WorkerId值，超过此值从0开始

var _ConnString = ""
var _Password = ""

const _WorkerIdIndexKey string = "IdGen:WorkerId:Index"        // redis 中的key
const _WorkerIdValueKeyPrefix string = "IdGen:WorkerId:Value:" // redis 中的key
const _WorkerIdFlag = "Y"                                      // IdGen:WorkerId:Value:xx 的值（将来可用 _token 替代）
const _Log = false                                             // 是否输出日志

func ValidateLocalWorkerId(workerId int) int {
	if workerId == _usingWorkerId {
		return 0
	} else {
		return -1
	}
}

func UnRegisterWorkerId() {
	if _usingWorkerId < 0 {
		return
	}

	_workerIdLock.Lock()
	_client.Del(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(_usingWorkerId))
	_usingWorkerId = -1
	_lifeIndex = -1
	_workerIdLock.Unlock()

	_client.Close()
}

func RegisterWorkerId(ip string, port int, password string, maxWorkerId int) int {
	// maxWorkerId不能小于0
	if maxWorkerId < 0 {
		return -2
	}

	// 如果当前已注册过 WorkerId，则先注销，并终止先前的自动续期线程
	if _usingWorkerId > -1 {
		UnRegisterWorkerId()
	}

	_MaxWorkerId = maxWorkerId
	_ConnString = ip + ":" + strconv.Itoa(port)
	_Password = password
	_client = newRedisClient()

	_, err := _client.Ping(_ctx).Result()
	if err != nil {
		panic("init redis error")
	} else {
		if _Log {
			fmt.Println("init redis ok")
		}
		defer _client.Close()
	}

	_loopCount = 0
	return getNextWorkerId()
}

func newRedisClient() *redis.Client {
	return redis.NewClient(&redis.Options{
		Addr:     _ConnString,
		Password: _Password,
		DB:       0,
		//PoolSize:     1000,
		//ReadTimeout:  time.Millisecond * time.Duration(100),
		//WriteTimeout: time.Millisecond * time.Duration(100),
		//IdleTimeout:  time.Second * time.Duration(60),
	})
}

func getNextWorkerId() int {
	// 获取当前 WorkerIdIndex
	r, err := _client.Incr(_ctx, _WorkerIdIndexKey).Result()
	if err != nil {
		return 0
	}

	candidateId := int(r)
	if _Log {
		fmt.Println("Begin candidateId:" + strconv.Itoa(candidateId))
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

			return getNextWorkerId()
		} else {
			// 如果有其它应用正在编辑，则本应用暂停200ms后，再继续
			time.Sleep(time.Duration(200) * time.Millisecond)

			if _Log {
				fmt.Println("not canReset loop")
			}

			return getNextWorkerId()
		}
	}

	if _Log {
		fmt.Println("candidateId:" + strconv.Itoa(candidateId))
	}

	if isAvailable(candidateId) {
		if _Log {
			fmt.Println("AA: isAvailable:" + strconv.Itoa(candidateId))
		}

		// 最新获得的 WorkerIdIndex，在 redis 中是可用状态
		setWorkerIdFlag(candidateId)
		_usingWorkerId = candidateId
		_loopCount = 0

		// 获取到可用 WorkerId 后，启用新线程，每隔 1/3个 _WorkerIdLifeTimeSeconds 时间，向服务器续期（延长一次 LifeTime）
		_lifeIndex++
		go extendWorkerIdLifeTime(_lifeIndex)

		return candidateId
	} else {
		if _Log {
			fmt.Println("BB: not isAvailable:" + strconv.Itoa(candidateId))
		}
		// 最新获得的 WorkerIdIndex，在 redis 中是不可用状态，则继续下一个 WorkerIdIndex
		return getNextWorkerId()
	}
}

func extendWorkerIdLifeTime(lifeIndex int) {
	var myLifeIndex = lifeIndex

	// 循环操作：间隔一定时间，刷新 WorkerId 在 redis 中的有效时间。
	for {
		time.Sleep(time.Duration(_WorkerIdLifeTimeSeconds/3) * time.Second)

		// 上锁操作，防止跟 UnRegisterWorkerId 操作重叠
		_workerIdLock.Lock()

		// 如果临时变量 myLifeIndex 不等于 全局变量 _lifeIndex，表明全局状态被修改，当前线程可终止，不应继续操作 redis
		if myLifeIndex != _lifeIndex {
			break
		}

		// 已经被注销，则终止（此步是上一步的二次验证）
		if _usingWorkerId < 0 {
			break
		}

		// 延长 redis 数据有效期
		extendWorkerIdFlag(_usingWorkerId)

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

func setWorkerIdFlag(index int) {
	_client.Set(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(index), _WorkerIdFlag, time.Duration(_WorkerIdLifeTimeSeconds)*time.Second)
}

func extendWorkerIdFlag(index int) {
	var client = newRedisClient()
	defer client.Close()
	client.Expire(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(index), time.Duration(_WorkerIdLifeTimeSeconds)*time.Second)
}

func canReset() bool {
	r, err := _client.Incr(_ctx, _WorkerIdValueKeyPrefix+"Edit").Result()
	if err != nil {
		return false
	}

	if _Log {
		fmt.Println("canReset:" + string(r))
	}

	return r != 1
}

func endReset() {
	// _client.Set(_WorkerIdValueKeyPrefix+"Edit", 0, time.Duration(2)*time.Second)
	_client.Set(_ctx, _WorkerIdValueKeyPrefix+"Edit", 0, 0)
}

func getWorkerIdFlag(index int) (string, bool) {
	r, err := _client.Get(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(index)).Result()
	if err != nil {
		return "", false
	}
	return r, true
}

func isAvailable(index int) bool {
	r, err := _client.Get(_ctx, _WorkerIdValueKeyPrefix+strconv.Itoa(index)).Result()

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
