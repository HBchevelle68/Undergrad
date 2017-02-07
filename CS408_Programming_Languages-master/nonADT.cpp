#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

struct Block{
	vector<char> use;
	vector<char> def;
	vector<char> in;
	vector<char> out;
	int successors[2];
	int num_successors;
	//vector<Block> successors;
};
struct Block barray[6];
void print_vector(vector<char> &v){
	for (int i = 0; i < v.size(); i++){
		printf("%c ", v[i]);
	}
	printf("\n");
}
void copy_to_from(vector<char> &to, vector<char> &from){
	to.clear();
	if (from.empty()){ return; }

	for (int i = 0; i < from.size(); i++){
		to.push_back(from[i]);
	}
	//printf("Printing Vector ");
	//print_vector(to);
	return;
}
void union_with_successors(struct Block &curB){
	vector<char> temp;
	if (curB.num_successors == 0){
		curB.out.clear();
		//printf("SUCCESSORS ARE EMPTY\n");
		return;
	}
	else if (curB.num_successors > 1){
		int successor1 = curB.successors[0];
		int successor2 = curB.successors[1];
		set_union(barray[successor1 - 1].in.begin(), barray[successor1 - 1].in.end(), barray[successor2 - 1].in.begin(), barray[successor2 - 1].in.end(), std::back_inserter(temp));
		copy_to_from(curB.out, temp);
		//printf("%s\n", );
		//curB.out = temp;
		return;
	}
	else{
		int successor1 = curB.successors[0];
		//print_vector(barray[successor1-1].in);
		copy_to_from(curB.out, barray[successor1 - 1].in);
	}
	return;
}

void weird_union(struct Block &curB){
	vector<char> temp;
	if (curB.out.empty()){
		copy_to_from(curB.in, curB.use);
		return;
	}
	else {
		temp = curB.out;
		for (int i = 0; i < temp.size(); i++){
			for (int j = 0; j < curB.def.size(); j++){
				if (temp[i] == curB.def[j]){
					temp.erase(temp.begin() + i);
					break;
				}
			}
		}
		//now union with "use" set and temp
		vector<char> temp3;
		vector<char> temp2;
		temp2 = curB.use;
		set_union(temp2.begin(), temp2.end(), temp.begin(), temp.end(), std::back_inserter(temp3));
		copy_to_from(curB.in, temp3);
		return;
	}
}
int compare_vectors(vector<char> &one, vector<char> &two ){
	if (one.size() != two.size()){return 0;}
	int flag = 0;
	for (int i = 0; i < one.size(); i++){
		for (int j = 0; j < two.size(); j++){
			if (one[i] == two[j]){
				flag = 1;
				break;
			}
			flag = 1;
		}
	}
}

int main(){

	struct Block B1;
	struct Block B2;
	struct Block B3;
	struct Block B4;
	struct Block B5;
	struct Block B6;

	B2.use.push_back('a');
	B2.use.push_back('b');
	B3.use.push_back('b');
	B3.use.push_back('d');
	B4.use.push_back('a');
	B4.use.push_back('b');
	B4.use.push_back('e');
	B5.use.push_back('a');
	B5.use.push_back('b');
	B5.use.push_back('c');
	B6.use.push_back('b');
	B6.use.push_back('d');

	B1.def.push_back('a');
	B1.def.push_back('b');
	B2.def.push_back('c');
	B2.def.push_back('d');
	B4.def.push_back('d');
	B5.def.push_back('e');
	B6.def.push_back('a');

	B1.successors[0] = 2;
	B2.successors[0] = 3;
	B2.successors[1] = 5;
	B3.successors[0] = 4;
	B3.successors[1] = 5;
	B4.successors[0] = 3;
	B5.successors[0] = 2;
	B5.successors[1] = 6;

	B1.num_successors = 1;
	B2.num_successors = 2;
	B3.num_successors = 2;
	B4.num_successors = 1;
	B5.num_successors = 2;
	B6.num_successors = 0;

	barray[0] = B1;
	barray[1] = B2;
	barray[2] = B3;
	barray[3] = B4;
	barray[4] = B5;
	barray[5] = B6;

	int iter = 1;
	bool change = true;
	while (change){
		change = false;
		vector<char>old;
		printf("Number of while loops %d\n", iter);
		for (int i = 0; i < 6; i++){

			printf("IN: ");
			print_vector(barray[i].in);
			printf("OUT: ");
			print_vector(barray[i].out);
			copy_to_from(old, barray[i].in);
			union_with_successors(barray[i]);
			weird_union(barray[i]);
			printf("IN after: ");
			print_vector(barray[i].in);
			printf("OUT after: ");
			print_vector(barray[i].out);
			printf("\n");
			if (barray[i].in != old){
				change = true;
			}
		}
		iter++;
	}
	return 0;
}
