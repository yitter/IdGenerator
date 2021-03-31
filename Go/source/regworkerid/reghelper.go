package regworkerid

import (
	"fmt"
	"github.com/go-redis/redis"
	"strconv"
	"sync"
	"time"
)

var Client *redis.Client

var _usingWorkerId int = -1
var _maxWorkerId int = 0
var _loopCount int = 0
var _liftIndex int = -1
var _workerIdLock sync.Mutex

const CurrentWidIndexKey string = "IdGen:WorkerId:Index"
const WidKeyPrefix string = "IdGen:WorkerId:Value:"
const WorkerIdFlag = "Y"           // WorkerId 存储标记
const WorkerIdLifeTimeSeconds = 15 // WorkerIdFlag 有效期（单位秒，最好是3的倍数）
const Log = false

func UnRegisterWorkerId() {
	if _usingWorkerId < 0 {
		return
	}
	_workerIdLock.Lock()
	Client.Del(WidKeyPrefix + strconv.Itoa(_usingWorkerId))
	_usingWorkerId = -1
	_liftIndex = -1
	_workerIdLock.Unlock()
}

func RegisterWorkerId(ip string, port int, password string, maxWorkerId int) int {
	// maxWorkerId不能小于0
	if maxWorkerId < 0 {
		return -1
	}

	// 如果当前已注册过 WorkerId，则先注销，并终止先前的自动续期线程
	if _usingWorkerId > -1 {
		UnRegisterWorkerId()
	}

	_maxWorkerId = maxWorkerId
	Client = redis.NewClient(&redis.Options{
		Addr:         string(ip) + ":" + strconv.Itoa(port),
		PoolSize:     1000,
		ReadTimeout:  time.Millisecond * time.Duration(100),
		WriteTimeout: time.Millisecond * time.Duration(100),
		IdleTimeout:  time.Second * time.Duration(60),
		Password:     password,
	})
	_, err := Client.Ping().Result()
	if err != nil {
		panic("init redis error")
	} else {
		if Log {
			fmt.Println("init redis ok")
		}
	}

	_loopCount = 0
	return getNextWorkerId()
}

func getNextWorkerId() int {
	// 获取当前 WorkerIdIndex
	// var currentId = int(Client.Incr(CurrentWidIndexKey).Val())
	r, err := Client.Incr(CurrentWidIndexKey).Result()
	if err != nil {
		return 0
	}

	currentId := int(r)
	if Log {
		fmt.Println("Begin currentId:" + strconv.Itoa(currentId))
	}

	// 如果 Index 大于最大值，则重置
	if currentId > _maxWorkerId {
		if canReset() {
			// 当前应用获得重置 WorkerIdIndex 的权限
			setWorkerIdIndex(-1)
			endReset() // 此步有可能不被执行？
			_loopCount++
			// 超过一定次数，直接终止操作
			if _loopCount > 10 {
				return -1
			}

			// if _loopCount > 2 {
			// 如果超过2个循环，则暂停1s
			time.Sleep(time.Duration(500*_loopCount) * time.Millisecond)
			//_loopCount = 0
			//}

			if Log {
				fmt.Println("canReset loop")
			}

			return getNextWorkerId()
		} else {
			// 如果有其它应用正在编辑，则本应用暂停1s后，再继续
			time.Sleep(time.Duration(1000) * time.Millisecond)

			if Log {
				fmt.Println("not canReset loop")
			}

			return getNextWorkerId()
		}
	}

	if Log {
		fmt.Println("currentId:" + strconv.Itoa(currentId))
	}

	if isAvailable(currentId) {
		if Log {
			fmt.Println("AA: isAvailable:" + strconv.Itoa(currentId))
		}

		// 最新获得的 WorkerIdIndex，在 redis 中是可用状态
		setWorkerIdFlag(currentId)
		_usingWorkerId = currentId

		// 获取到可用 WorkerId 后，启用新线程，每隔 1/3个 WorkerIdLifeTimeSeconds 时间，向服务器续期（延长一次 LifeTime）
		_liftIndex++
		go extendWorkerIdLifeTime(_liftIndex)

		return currentId
	} else {
		if Log {
			fmt.Println("BB: not isAvailable:" + strconv.Itoa(currentId))
		}
		// 最新获得的 WorkerIdIndex，在 redis 中是不可用状态，则继续下一个 WorkerIdIndex
		return getNextWorkerId()
	}
}

func extendWorkerIdLifeTime(lifeIndex int) {
	var index = lifeIndex
	for {
		time.Sleep(time.Duration(WorkerIdLifeTimeSeconds/3) * time.Millisecond)

		_workerIdLock.Lock()
		if index != _liftIndex {
			// 如果临时变量 index 不等于 全局变量	_liftIndex，表明全局状态被修改，当前线程可终止
			break
		}

		// 已经被注销，则终止（此步是上一步的二次验证）
		if _usingWorkerId < 0 {
			break
		}

		extendWorkerIdFlag(_usingWorkerId)
		_workerIdLock.Unlock()
	}
}

func get(key string) (string, bool) {
	r, err := Client.Get(key).Result()
	if err != nil {
		return "", false
	}
	return r, true
}

func set(key string, val string, expTime int32) {
	Client.Set(key, val, time.Duration(expTime)*time.Second)
}

func setWorkerIdIndex(val int) {
	Client.Set(CurrentWidIndexKey, val, 0)
}

func setWorkerIdFlag(index int) {
	Client.Set(WidKeyPrefix+strconv.Itoa(index), WorkerIdFlag, time.Duration(WorkerIdLifeTimeSeconds)*time.Second)
}

func extendWorkerIdFlag(index int) {
	Client.Expire(WidKeyPrefix+strconv.Itoa(index), time.Duration(WorkerIdLifeTimeSeconds)*time.Second)
}

func canReset() bool {
	r, err := Client.Incr(WidKeyPrefix + "Edit").Result()
	if err != nil {
		return false
	}

	if Log {
		fmt.Println("canReset:" + string(r))
	}

	return r != 1
}

func endReset() {
	// Client.Set(WidKeyPrefix+"Edit", 0, time.Duration(2)*time.Second)
	Client.Set(WidKeyPrefix+"Edit", 0, 0)
}

func getWorkerIdFlag(index int) (string, bool) {
	r, err := Client.Get(WidKeyPrefix + strconv.Itoa(index)).Result()
	if err != nil {
		return "", false
	}
	return r, true
}

func isAvailable(index int) bool {
	r, err := Client.Get(WidKeyPrefix + strconv.Itoa(index)).Result()

	if Log {
		fmt.Println("XX isAvailable:" + r)
		fmt.Println("YY isAvailable:" + err.Error())
	}

	if err != nil {
		if err.Error() == "redis: nil" {
			return true
		}
		return false
	}
	return r != WorkerIdFlag
}
