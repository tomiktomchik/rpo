package main

import (
	"./decoder"
	"./encoder"
	"./receiver"
	"./source"
)


func main(){
	code := source.OpenFile()
	symFreqs := make(map[rune]int) //создание словаря для буква - количество её повторения в тексте
	for _, c := range code { // подсчет количества повторений каждого символа из текста
		symFreqs[c]++
	}
	var tree []encoder.Coded // переменная для бинарного дерева закодированных символов
							// данная переменная принимает тип структуры Coded из пакета encoder
	tree = encoder.Encode(symFreqs) // кодирование, то есть получение бинарного дерева

	var codes []string // массив кодов (индекс значений == индекс кода)
	var values []rune // массив значений
	codesMap := make(map[rune]string) // в словаре код == значение, заполнение массива и словаря из бинарного дерева
	for _, c := range tree {
		codes = append(codes, c.Code)
		values = append(values, c.Sym)
		codesMap[c.Sym] = c.Code
	}
	ans := ""
	for _, c := range code { // перевод строки в код
		ans += codesMap[c]
	}
	str := decoder.Decode(ans, codes, values) // декодирование
	receiver.WriteFile([]byte(ans), "outcode.txt")
	receiver.WriteFile([]byte(str), "output.txt")
}