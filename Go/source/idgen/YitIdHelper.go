package idgen

import (
	"sync"
	"time"
)

var singletonMutex sync.Mutex
var idGenerator *DefaultIdGenerator

// SetIdGenerator .
func SetIdGenerator(options *IdGeneratorOptions) {
	singletonMutex.Lock()
	idGenerator = NewDefaultIdGenerator(options)
	singletonMutex.Unlock()
}

// NextId .
func NextId() int64 {
	if idGenerator == nil {
		singletonMutex.Lock()
		defer singletonMutex.Unlock()
		if idGenerator == nil {
			options := NewIdGeneratorOptions(1)
			idGenerator = NewDefaultIdGenerator(options)
		}
	}

	return idGenerator.NewLong()
}

func ExtractTime(id int64) time.Time {
	return idGenerator.ExtractTime(id)
}
