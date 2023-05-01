// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь:
#include <iostream>
#include <string>

using namespace std;

int main(){
string num;
int k=0;
for(int i=1; i<1001;++i){
	num=to_string(i);
	for(char c:num) if(c=='3') ++k;
	}
cout <<"В ряде от 1 до 1000 - "s <<k <<" чисел содержат хотябы одну цифру 3"s <<endl;
}

// Закомитьте изменения и отправьте их в свой репозиторий.
