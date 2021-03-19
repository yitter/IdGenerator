package contract

import "fmt"

type IdGeneratorException struct {
	message string
	error   error
}

func (e IdGeneratorException) IdGeneratorException(message ...interface{}) {
	fmt.Println(message)
}
