package receiver

import (
	"io/ioutil"
	"log"
)

func WriteFile(str []byte, fileName string) { // запись в файл
	err := ioutil.WriteFile(fileName, str, 0644) // код 0644 - запись и чтение файла
	if err != nil {
		log.Fatal(err)
	}
}