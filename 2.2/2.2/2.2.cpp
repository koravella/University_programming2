#include <iostream>
#include <list>
#include <fstream>
#include <conio.h>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <iomanip>

using namespace std;

class Animals;

class Game {
	int N;
	int M;
	list <Animals*> beasts;
	void copy_Game(Game& that);
	void clear_Game();
public:
	Game();
	Game(const int a, const int b);
	void append(Animals* new_animal);
	Game(Game& that);
	Game& operator= (Game& that);
	~Game();
	void print();
	void step();
	int carnivory(const Animals& ani);
	int start();
	void rand_beast(const int N, const int M, const int Max_shift, int& x, int& y, int& start, int& shift);
};
class Animals {
protected:
	int age;
	int satiety;
	pair <int, int> coord;
	int direct, ind_shift, temp_ind_shift;
	void real_move(const int max_x, const int max_y, const int k);
public:
	Animals() = default;
	Animals(const int x, const int y, const int start, const int shift);
	virtual void move(const int max_x, const int max_y) = 0;
	virtual bool can_be_fed() const = 0;
	pair <int, int> get_coord() const;
	virtual void feeding(Game& zoo) = 0;
	void kill() const;
	void aging();
	virtual void reproduction(Game& zoo)=0;
	virtual bool extinction() const = 0;
	virtual int get_species() const = 0;

};
class Wolf : public Animals {
public:
	Wolf() = default;
	Wolf(const int x, const int y, const int start, const int shift);
	void move(const int max_x, const int max_y);
	bool can_be_fed() const;
	void feeding(Game& zoo);
	virtual void reproduction(Game& zoo) = 0;
	bool extinction() const;
	virtual int get_species() const = 0;
};
class Rabbit : public Animals {
public:
	Rabbit() = default;
	Rabbit(const int x, const int y, const int start, const int shift);
	void move(const int max_x, const int max_y);
	bool can_be_fed() const;
	void feeding(Game& zoo);
	void reproduction(Game& zoo);
	bool extinction() const;
	int get_species() const;
};
class Wolf1: public Wolf {
public:
	Wolf1() = default;
	Wolf1(const int x, const int y, const int start, const int shift);
	int get_species() const;
	void reproduction(Game& zoo);
};
class Wolf2: public Wolf {
public:
	Wolf2() = default;
	Wolf2(const int x, const int y, const int start, const int shift);
	int get_species() const;
	void reproduction(Game& zoo);
};

Animals::Animals(const int x, const int y, const int start, const int shift): age(0), satiety(0), coord(x, y), direct(start), ind_shift(shift), temp_ind_shift(shift){

}
void Animals::real_move(const int max_x, const int max_y, const int k) {
	switch (direct) {
	case 0:
		this->coord.second = (this->coord.second - k + max_y) % max_y;
		break;
	case 1:
		this->coord.first = (this->coord.first + k) % max_x;
		break;
	case 2:
		this->coord.second = (this->coord.second + k) % max_y;
		break;
	case 3:
		this->coord.first = (this->coord.first - k + max_x) % max_x;
		break;
	}
	--this->temp_ind_shift;
	if (this->temp_ind_shift==0) {
		this->direct = (this->direct + 1) % 4;
		temp_ind_shift = ind_shift;
	}
}
pair <int, int> Animals::get_coord() const {
	pair <int, int> res(this->coord);
	return res;
}
void Animals::kill() const {
	delete this;
}
void Animals::aging() {
	this->age += 1;
}

Wolf::Wolf(const int x, const int y, const int start, const int shift) : Animals(x, y, start, shift) {

}
void Wolf::move(const int max_x, const int max_y) {
	this->real_move(max_x, max_y, 2);
}
bool Wolf::can_be_fed() const {
	return false;
}
void Wolf::feeding(Game& zoo) {
	this->satiety += zoo.carnivory(*this);
}
bool Wolf::extinction() const {
	if (this->age == 15) {
		this->kill();
		return true;
	}
	return false;
}

Rabbit::Rabbit(const int x, const int y, const int start, const int shift) : Animals(x, y, start, shift) {

}
void Rabbit::move(const int max_x, const int max_y) {
	real_move(max_x, max_y, 1);
}
bool Rabbit::can_be_fed() const {
	return true;
}
void Rabbit::feeding(Game& zoo) {

}
void Rabbit::reproduction(Game& zoo) {
	if (this->age == 5 || this->age == 10) {
		zoo.append(new Rabbit(this->coord.first, this->coord.second, this->direct, this->ind_shift));
	}
}
bool Rabbit::extinction() const {
	if (this->age == 10) {
		this->kill();
		return true;
	}
	return false;
}
int Rabbit::get_species() const {
	return 0;
}

Game::Game(): N(1), M(1) {

}
Game::Game(const int a, const int b): N(a), M(b) {

}
void Game::append(Animals* new_animal) {
	this->beasts.push_back(new_animal);
}
void Game::copy_Game(Game& that) {
	this->N = that.N;
	this->M = that.M;
	list< Animals* >::iterator it;
	list< Animals* >::reverse_iterator it1;
	for (it = that.beasts.begin(); it != that.beasts.end(); it++) {
		if ((*it)->get_species() == 0) {
			this->beasts.push_back(new Rabbit);
			it1 = this->beasts.rbegin();
			*(*it1) = *(*it);
		}
		else if ((*it)->get_species() == 1) {
			this->beasts.push_back(new Wolf1);
			it1 = this->beasts.rbegin();
			*(*it1) = *(*it);
		}
		else if ((*it)->get_species() == 2) {
			this->beasts.push_back(new Wolf2);
			it1 = this->beasts.rbegin();
			*(*it1) = *(*it);
		}
	}
}
Game::Game(Game& that) {
	this->copy_Game(that);
}
Game& Game::operator= (Game& that) {
	if (this != &that) {
		this->clear_Game();
		this->copy_Game(that);
	}
	return *this;
}
void Game::clear_Game() {
	list< Animals* >::iterator it;
	for (it = this->beasts.begin(); it != this->beasts.end(); it++) {
		delete (*it);
	}
	this->beasts.clear();
}
Game::~Game() {
	this->clear_Game();
}
void Game::print() {
	system("cls");
	int** arr = new int* [M];
	for (int i = 0; i < M; i++) {
		arr[i] = new int[N];
	}
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			arr[i][j] = 0;
		}
	}
	list< Animals* >::iterator it;
	for (it = beasts.begin(); it != beasts.end(); it++) {
		pair<int, int> p = (*it)->get_coord();
		if ((*it)->can_be_fed()) {
			arr[p.second][p.first] += 1;
		}
		else {
			arr[p.second][p.first] -= 1;
		}
	}
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			if (arr[i][j] == 0) {
				cout << setw(2) << "#";
			}
			else {
				cout << setw(2) << arr[i][j];
			}
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < M; i++) {
		delete[] arr[i];
	}
	delete[] arr;
	_getch();
}
void Game::step() {
	list< Animals* >::iterator it;
	for (it = beasts.begin(); it != beasts.end(); it++) {
		(*it)->move(this->N, this->M);
	}
	for (it = beasts.begin(); it != beasts.end(); it++) {
		(*it)->feeding(*this);
	}
	for (it = beasts.begin(); it != beasts.end(); it++) {
		(*it)->aging();
	}
	for (it = beasts.begin(); it != beasts.end(); ++it) {
		(*it)->reproduction(*this);
	}
	for (it = beasts.begin(); it != beasts.end();) {
		if ((*it)->extinction()) {
			it = beasts.erase(it);
		}
		else {
			++it;
		}
	}
}
int Game::carnivory(const Animals& ani) {
	list< Animals* >::iterator it;
	int sum = 0;
	for (it = beasts.begin(); it != beasts.end();) {
		if (ani.get_coord() == (*it)->get_coord()) {
			if ((*it)->can_be_fed() == true) {
				(*it)->kill();
				it = beasts.erase(it);
				sum += 1;
			}
			else if (ani.get_species() != (*it)->get_species()) {
				bool r = rand() % 2;
				if (r) {
					(*it)->kill();
					it = beasts.erase(it);
				}
				else ++it;
			}
			else ++it;
		}
		else ++it;
	}
	return sum;
}
int Game::start() {
	cout << "Random generation - Y, from file - N ";
	char act = _getch();
	int N, M, T, R, W1, W2;
	int x, y, start, shift;
	if (act == 'N' || act == 'n') {
		ifstream fin;
		fin.open("input.txt");
		fin >> N >> M >> T >> R >> W1 >> W2;
		Game zoo_temp(N, M);
		for (int i = 0; i < R; i++) {
			fin >> x >> y >> start >> shift;
			zoo_temp.append(new Rabbit(x, y, start, shift));
		}
		for (int i = 0; i < W1; i++) {
			fin >> x >> y >> start >> shift;
			zoo_temp.append(new Wolf1(x, y, start, shift));
		}
		for (int i = 0; i < W2; i++) {
			fin >> x >> y >> start >> shift;
			zoo_temp.append(new Wolf2(x, y, start, shift));
		}
		*this = zoo_temp;
		fin.close();
	}
	else if (act == 'Y' || act == 'y') {
		system("cls");
		cout << "N, M, T, R, W1, W2: ";
		cin >> N >> M >> T >> R >> W1 >> W2;
		int Max_shift;
		cout << "maximum moves before turning: ";
		cin >> Max_shift;
		Game zoo_temp(N, M);
		for (int i = 0; i < R; i++) {
			rand_beast(N, M, Max_shift, x, y, start, shift);
			zoo_temp.append(new Rabbit(x, y, start, shift));
		}
		for (int i = 0; i < W1; i++) {
			rand_beast(N, M, Max_shift, x, y, start, shift);
			zoo_temp.append(new Wolf1(x, y, start, shift));
		}
		for (int i = 0; i < W2; i++) {
			rand_beast(N, M, Max_shift, x, y, start, shift);
			zoo_temp.append(new Wolf2(x, y, start, shift));
		}
		*this = zoo_temp;
	}
	else {
		assert(0);
	}
	return T;
}
void Game::rand_beast(const int N, const int M, const int Max_shift, int& x, int& y, int& start, int& shift) {
	x = rand() % N;
	y = rand() % M;
	start = rand() % 4;
	shift = 1 + rand() % Max_shift;
	cout << x << " " << y << " " << start << " " << shift << endl;
}

Wolf1::Wolf1(const int x, const int y, const int start, const int shift): Wolf(x, y, start, shift) {

}
int Wolf1::get_species() const {
	return 1;
}
void Wolf1::reproduction(Game& zoo) {
	while (this->satiety >= 2) {
		zoo.append(new Wolf1(this->coord.first, this->coord.second, this->direct, this->ind_shift));
		this->satiety -= 2;
	}
}

Wolf2::Wolf2(const int x, const int y, const int start, const int shift) : Wolf(x, y, start, shift) {

}
int Wolf2::get_species() const {
	return 2;
}
void Wolf2::reproduction(Game& zoo) {
	while (this->satiety >= 2) {
		zoo.append(new Wolf2(this->coord.first, this->coord.second, this->direct, this->ind_shift));
		this->satiety -= 2;
	}
}

int main()
{
	srand(time(0));
	Game zoo;
	int T;
	T=zoo.start();
	while (T) {
		--T;
		zoo.step();
		zoo.print();
	}
	return 0;
}

