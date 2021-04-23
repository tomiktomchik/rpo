package decoder

// функция декодирования строки
func Decode (dec string, codes []string, values []rune) string {
	count := 0
	var l int = 0
	r := len(codes) - 1
	cur := ""
	ans := ""
	for _, i := range dec { //поиск по бинарному дереву с учетом каждого элемента, сдвигаются границы r - правая, l - левая
		cur += string(i)
		if string(i) == "0" {
			for j := l; j <= r; j++ {
				if string(codes[j][count]) != "0" {
					r = j - 1
					count += 1
					break
				}
			}
		}
		if string(i) == "1" {
			for j := r; j >= l; j-- {
				if string(codes[j][count]) != "1" {
					l = j+1
					count += 1
					break
				}
			}
		}
		if l == r { // когда левая и правая границы равны
			ans += string(values[l])
			cur = ""
			r = len(codes) -1
			l = 0
			count = 0
		}
	}
	return ans
}