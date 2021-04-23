package source

import (
	"io/ioutil"
	"log"
	"strings"
)

func OpenFile() string{
	f, err := ioutil.ReadFile("input.txt")
	if err != nil { // возникла ли ошибка перед чтением
		log.Fatal(err)
	}
	content := strings.ToLower(string(f)) // убираем заглавные буквы

	return content
}