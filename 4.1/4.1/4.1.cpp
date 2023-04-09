#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <stack>
#include <set>
#include <deque>
#include <map>
#include <cmath>

using namespace std;
const char eps = 'E'; //пустой переход

class Automaton {
	int num_Q;
	int start_q;
	set<char> alphabet;
	vector<int> end;
	vector<list<pair<char, int>>> transit;
	bool DKA;
	Automaton(const string& str, bool fict); //автомат из символьной строки, где есть только конкатенация
	set<int> closing(set<int>& sset);
	int matching(set<int>& sset);
public:
	Automaton();
	Automaton(const int N, const int start, const vector<int>& t_end, vector<string> t_transit);
	Automaton(const string& t_reg_expr);
	bool recognize(const string& str);
	void determinization();
	Automaton operator+(const Automaton& that) const; //конкатенация
	Automaton& operator+();//"звездочка Клини" (хотя бы один)
	Automaton& operator*();//звездочка Клини
	Automaton operator|(const Automaton& that) const; //объединение
	Automaton(Automaton& that) = default;
	Automaton& operator=(Automaton& that) = default;
	Automaton(Automaton&& that);
	Automaton& operator=(Automaton&& that);
	void print(ostream& fout);
};

Automaton::Automaton():num_Q(0), start_q(0), DKA(false){}
Automaton::Automaton(const int N, const int start, const vector<int>& t_end, vector<string> t_transit):num_Q(N), start_q(start), end(t_end), DKA(false) {
	int from, to;
	char value;
	this->transit.resize(N);
	for (int i = 0; i < t_transit.size(); i++) {
		from = stoi(t_transit[i]);
		t_transit[i] = t_transit[i].substr(t_transit[i].find(" ")+1, string::npos);
		to = stoi(t_transit[i]);
		t_transit[i] = t_transit[i].substr(t_transit[i].find(" ")+1, string::npos);
		value = t_transit[i][0];
		this->transit[from].push_back(make_pair(value, to));
		this->alphabet.insert(value);
	}
}
Automaton::Automaton(const string& t_reg_expr) {
	stack<char> st_oper;
	string reg_expr = "(" + t_reg_expr + ")"; //добавление скобочек для упрощения обработки
	stack<Automaton*> st_aut;
	string auxiliary = "()|*+";
	for (int cur = 0; cur < reg_expr.size(); ) {
		if (auxiliary.find_first_of(reg_expr[cur], 0) == string::npos) {
			if (cur != 0 && reg_expr[cur - 1] != '|' && reg_expr[cur - 1] != '(') {
				st_oper.push('+');
			}
			string t_str;
			int tmp = reg_expr.find_first_of("()|", cur, 3);
			t_str = reg_expr.substr(cur, tmp-cur);
			st_aut.push(new Automaton(t_str, 0));
			cur += t_str.size();
		}
		else {
			switch (reg_expr[cur]) {
			case '(': {
				if (cur != 0 && reg_expr[cur-1]!='|' && reg_expr[cur - 1] != '(') {
					st_oper.push('+');
				}
				st_oper.push('(');
				break;
			}
			case '|': {
				st_oper.push('|');
				break;
			}
			case '*': {
				Automaton* tmp = st_aut.top();
				st_aut.pop();
				+(*tmp);
				st_aut.push(tmp);
				break;
			}
			case ')': {
				char c = st_oper.top();
				while (c!= '(') {
					switch (c) {
					case '+': {
						Automaton* tmp1 = st_aut.top(), *tmp2;
						st_aut.pop();
						tmp2 = st_aut.top();
						st_aut.pop();
						Automaton* res = new Automaton;
						*res = *tmp2 + *tmp1;
						st_aut.push(res);
						delete tmp1;
						delete tmp2;
						break;
					}
					case '|': {
						Automaton* tmp1 = st_aut.top(), * tmp2;
						st_aut.pop();
						tmp2 = st_aut.top();
						st_aut.pop();
						Automaton* res = new Automaton;
						*res = *tmp1 | *tmp2;
						st_aut.push(res);
						delete tmp1;
						delete tmp2;
						break;
					}
					}
					st_oper.pop();
					c = st_oper.top();
				}
				st_oper.pop();
				break;
			}
			}
			cur++;
		}
	}
	Automaton* res;
	if (!st_aut.empty()) {
		res = st_aut.top();
		st_aut.pop();
	}
	else {
		res = new Automaton;
	}
	*this = *res;
	delete res;
}
Automaton::Automaton(const string& str, bool fict):num_Q(str.size()+1), start_q(0), DKA(false) {
	this->end.push_back(num_Q - 1);
	this->transit.resize(num_Q);
	for (int i = 0; i < num_Q - 1; i++) {
		this->transit[i].push_back(make_pair(str[i], i + 1));
		this->alphabet.insert(str[i]);
	}
}
bool Automaton::recognize(const string& str) {
	this->determinization();
	int cur = this->start_q;
	int flag;
	for (int i = 0; i < str.size(); i++) {
		flag = 0;
		for (auto it = this->transit[cur].begin(); it != this->transit[cur].end(); it++) {
			if (it->first == str[i]) {
				cur = it->second;
				flag = 1;
				break;
			}
		}
		if (!flag) {
			return false;
		}
	}
	for (int i = 0; i < end.size(); i++) {
		if (end[i] == cur)
			return true;
	}
	return false;
}
Automaton Automaton::operator+(const Automaton& that) const {
	Automaton res;
	res.DKA=false;
	res.alphabet = this->alphabet;
	res.alphabet.insert(that.alphabet.begin(), that.alphabet.end());
	res.alphabet.insert(eps);
	res.num_Q = this->num_Q + that.num_Q;
	res.start_q = this->start_q;
	int offset = this->num_Q;
	for (int i = 0; i < that.end.size(); i++) {
		res.end.push_back(that.end[i] + offset);
	}
	res.transit = this->transit;
	res.transit.resize(res.num_Q);
	for (int i = 0; i < this->end.size(); i++) {
		res.transit[this->end[i]].push_back(make_pair(eps, that.start_q + offset));
	}
	for (int i = 0; i < that.transit.size(); i++) {
		for (auto it = that.transit[i].begin(); it != that.transit[i].end(); it++) {
			res.transit[i + offset].push_back(make_pair(it->first, it->second + offset));
		}
	}
	return res;
}
Automaton& Automaton::operator+() {
	Automaton res;
	res.DKA = false;
	res.alphabet = this->alphabet;
	res.alphabet.insert(eps);
	res.num_Q = this->num_Q;
	res.start_q = this->start_q;
	res.end = this->end;
	res.transit = this->transit;
	res.transit.resize(res.num_Q);
	for (int i = 0; i < this->end.size(); i++) {
		res.transit[this->end[i]].push_back(make_pair(eps, res.start_q));
	}
	*this = res;
	return *this;
}
Automaton& Automaton::operator*() {
	Automaton res;
	res.DKA = false;
	res.alphabet = this->alphabet;
	res.alphabet.insert(eps);
	res.num_Q = this->num_Q + 1;
	res.start_q = res.num_Q - 1;
	res.end = this->end;
	res.end.push_back(res.start_q);
	res.transit = this->transit;
	res.transit.resize(res.num_Q);
	for (int i = 0; i < this->end.size(); i++) {
		res.transit[this->end[i]].push_back(make_pair(eps, res.start_q));
	}
	res.transit[res.start_q].push_back(make_pair(eps, this->start_q));
	*this = res;
	return *this;
}
Automaton Automaton::operator|(const Automaton& that) const {
	Automaton res;
	res.DKA = false;
	res.alphabet = this->alphabet;
	res.alphabet.insert(that.alphabet.begin(), that.alphabet.end());
	res.alphabet.insert(eps);
	res.num_Q = this->num_Q + that.num_Q + 1;
	res.start_q = this->num_Q;
	res.end = this->end;
	int offset = this->num_Q + 1;
	for (int i = 0; i < that.end.size(); i++) {
		res.end.push_back(that.end[i] + offset);
	}
	res.transit = this->transit;
	res.transit.resize(res.num_Q);
	res.transit[res.start_q].push_back(make_pair(eps, this->start_q));
	res.transit[res.start_q].push_back(make_pair(eps, that.start_q + offset));
	for (int i = 0; i < that.transit.size(); i++) {
		for (auto it = that.transit[i].begin(); it != that.transit[i].end(); it++) {
			res.transit[i + offset].push_back(make_pair(it->first, it->second + offset));
		}
	}
	return res;
}
Automaton::Automaton(Automaton&& that):num_Q(that.num_Q), start_q(that.start_q), alphabet(move(that.alphabet)), end(move(that.end)), transit(move(that.transit)), DKA(that.DKA) {}
Automaton& Automaton::operator=(Automaton&& that) {
	this->num_Q = that.num_Q;
	this->start_q = that.start_q;
	this->alphabet = move(that.alphabet);
	this->end = move(that.end);
	this->transit = move(that.transit);
	this->DKA = that.DKA;
	return *this;
}
void Automaton::print(ostream& fout) {
	fout <<"Num: "<< num_Q << endl;
	fout <<"Start: "<< start_q << endl <<"End: ";
	for (auto it : this->end) {
		cout << it << " ";
	}
	fout << endl<<"Alphabet: ";
	for (auto it : this->alphabet) {
		fout << it << " ";
	}
	fout << endl;
	for (int i = 0; i < this->transit.size(); i++) {
		for (auto it : this->transit[i]) {
			fout << i << " ";
			fout << it.first;
			fout << " " << it.second << " " << endl;
		}
	}
}
void Automaton::determinization() {
	if (this->DKA) {
		return;
	}
	set<int> sset, res_start;
	Automaton res;
	res.num_Q = pow(2, this->num_Q);
	res.alphabet = this->alphabet;
	res.alphabet.erase(eps);
	sset.insert(this->start_q);
	res_start = closing(sset);
	res.start_q = matching(res_start);
	res.transit.resize(res.num_Q);
	deque<set<int>> deq;
	deq.push_back(res_start);

	while (!deq.empty()) {
		sset = deq.front();
		for (auto it5 : this->end) {
			if (sset.find(it5) != sset.end()) {
				res.end.push_back(matching(sset));
				break;
			}
		}
		for (auto it1 : this->alphabet) {
			if (it1 != eps) {
				set<int> tmp_set, cls_set;
				for (auto it2 : sset) {
					for (auto it3 : this->transit[it2]) {
						if (it3.first == it1) {
							cls_set.insert(it3.second);
						}
					}
					cls_set = closing(cls_set);
					tmp_set.insert(cls_set.begin(), cls_set.end());
				}
				int from = matching(sset), to = matching(tmp_set);
				res.transit[from].push_back(make_pair(it1, to));
				int flag = 0;
				if (!res.transit[to].empty()) {
					flag = 1;
				}
				else {
					for (auto it4 : deq) {
						if (it4 == tmp_set) {
							flag = 1;
							break;
						}
					}
				}
				if (!flag) {
					deq.push_back(tmp_set);
				}
			}
		}
		deq.pop_front();
	}

	vector<int> res_end;
	vector<list<pair<char, int>>> res_transit;
	map<int, int> j_pos;
	int k = 0;
	for (int i = 0; i < res.transit.size(); i++) {
		if (!res.transit[i].empty()) {
			j_pos[i] = k;
			++k;
		}
	}
	res_transit.resize(j_pos.size());
	for (int i = 0; i < res.transit.size(); i++) {
		for (auto it : res.transit[i]) {
			res_transit[j_pos[i]].push_back(make_pair(it.first, j_pos[it.second]));
		}
	}
	for (int i = 0; i < res.end.size(); i++) {
		res_end.push_back(j_pos[res.end[i]]);
	}
	res.num_Q = k;
	res.start_q = j_pos[res.start_q];
	res.end = res_end;
	res.transit = res_transit;
	res.DKA = true;
	*this = res;
}
set<int> Automaton::closing(set<int>& sset) {
	set<int> res;
	deque<int> deq;
	for (auto it : sset) {
		deq.push_back(it);
	}
	while (!deq.empty()) {
		int q = deq.front();
		deq.pop_front();
		res.insert(q);
		for (auto it : transit[q]) {
			if (it.first == eps) {
				deq.push_back(it.second);
			}
		}
	}
	return res;
}
int Automaton::matching(set<int>& sset) {
	int res=0;
	for (auto it : sset) {
		res += pow(2, it);
	}
	return res;
}

void DKA_recognize(istream& fin, ostream& fout) {
	int N, start, num_end, p, T;
	fin >> N >> start >> num_end;
	vector<int> t_end;
	for (int i = 0; i < num_end; i++) {
		int temp;
		fin >> temp;
		t_end.push_back(temp);
	}
	fin >> p;
	vector<string> t_transit;
	string temp;
	getline(fin, temp);
	for (int i = 0; i < p; i++) {
		getline(fin, temp);
		t_transit.push_back(temp);
	}
	Automaton DKA(N, start, t_end, t_transit);
	vector<bool> res;
	fin >> T;
	res.resize(T);
	for (int i = 0; i < T; i++) {
		fin >> temp;
		res[i] = (DKA.recognize(temp));
	}
	for (int i = 0; i < T; i++) {
		if (res[i]) {
			fout << "YES" << endl;
		}
		else {
			fout << "NO" << endl;
		}
	}
}
int main()
{
	ifstream fin;
	fin.open("input.txt");
	ofstream fout;
	fout.open("output.txt");
	DKA_recognize(fin, fout);
	fin.close();
	fout.close();
}
