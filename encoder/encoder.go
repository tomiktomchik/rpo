package encoder

import (
	"container/heap"
)

type Coded struct {
	Sym  rune
	Code string
}

type Counted struct {
	total int
	syms []Coded
}

type cHeap []Counted

// satisfy heap.Interface
func (c cHeap) Len() int           { return len(c) } // вычисление длины
func (c cHeap) Less(i, j int) bool { return c[i].total < c[j].total } //вычисление нименьшего
func (c cHeap) Swap(i, j int)      { c[i], c[j] = c[j], c[i] } // меняет местами лементы
func (c *cHeap) Push(ele interface{}) { //добавляет элемент
	*c = append(*c, ele.(Counted))
}
func (c *cHeap) Pop() (popped interface{}) { //удаление элемента
	popped = (*c)[len(*c)-1]
	*c = (*c)[:len(*c)-1]
	return
}

func Encode(sym2freq map[rune]int) []Coded {
	var ch cHeap
	for sym, freq := range sym2freq {
		ch = append(ch, Counted{freq, []Coded{{Sym: sym}}})

	}
	heap.Init(&ch)
	for len(ch) > 1 {
		a := heap.Pop(&ch).(Counted)
		b := heap.Pop(&ch).(Counted)

		for i, c := range a.syms {
			a.syms[i].Code = "0" + c.Code
		}
		for i, c := range b.syms {
			b.syms[i].Code = "1" + c.Code

		}
		heap.Push(&ch, Counted{a.total + b.total, append(a.syms, b.syms...)})
	}
	return heap.Pop(&ch).(Counted).syms
}
