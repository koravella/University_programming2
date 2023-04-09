#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <fstream>
#include <new>

class Matrix {
	class Vector {
		int* data;
		int size;
		Vector();
	public:
		void Vector_init(const int n);
		Vector(const int n);
		int& operator[](const int index);
		int operator[](const int index) const;
		Vector(const Vector& that);
		~Vector();
	};
	Vector* rows;
	int sz;
	void init(const int n);
	void copy(const Matrix& that);
	void delete_arr();
public:
	Matrix();
	Matrix(const int n);
	Matrix(const int n, const int k);
	Matrix(const int, std::ifstream& fin);
	Matrix(const Matrix& that);
	Matrix operator+ (const Matrix& that) const;
	Matrix operator* (const Matrix& that) const;
	Matrix& operator= (const Matrix& that);
	Vector& operator[] (const int index) const;
	Matrix& trans();
	void print(std::ofstream& fout);
	~Matrix();

};
Matrix::Vector& Matrix::operator[](const int index) const {
	if (index >= 0 && index < this->sz) {
		return this->rows[index];
	}
	else {
		std::cout << "invalid index_1!";
		assert(0);
	}
}
int& Matrix::Vector::operator[](const int index) {
	if (index >= 0 && index < this->size) {
		return this->data[index];
	}
	else {
		std::cout << "invalid index_2!";
		assert(0);
	}
}
int Matrix::Vector::operator[](const int index) const {
	if (index >= 0 && index < this->size) {
		return this->data[index];
	}
	else {
		std::cout << "invalid index_2!";
		assert(0);
	}
}
void Matrix::init(const int n) {
	this->sz = n;
	char* arr = new char[sizeof(Vector) * this->sz];
	this->rows = (Vector*)arr;
	for (int i = 0; i < this->sz; i++) {
		new (rows + i) Vector(this->sz);
	}
}
void Matrix::copy(const Matrix& that) {
	this->init(that.sz);
	for (int i = 0; i < this->sz; i++) {
		for (int j = 0; j < this->sz; j++) {
			(*this)[i][j] = that[i][j];
		}
	}
}
void Matrix::delete_arr() {
	for (int i = 0; i < this->sz; i++) {
		this->rows[i].~Vector();
	}
	delete[] (char*) this->rows;
}
Matrix::Matrix() {
	this->init(1);
	(*this)[0][0] = 1;
}
Matrix::Matrix(const int n) {
	this->init(n);
	for (int i = 0; i < this->sz; i++) {
		for (int j = 0; j < this->sz; j++) {
			(*this)[i][j] = rand();
		}
	}
}
Matrix::Matrix(const int n, const int k) {
	this->init(n);
	for (int i = 0; i < this->sz; i++) {
		for (int j = 0; j < this->sz; j++) {
			if (i == j) {
				(*this)[i][j] = k;
			}
			else {
				(*this)[i][j] = 0;
			}
		}
	}
}
Matrix::Matrix(const int n, std::ifstream& fin) {
	this->init(n);
	for (int i = 0; i < this->sz; i++) {
		for (int j = 0; j < this->sz; j++) {
			fin >> (*this)[i][j];
		}
	}
}
Matrix::Matrix(const Matrix& that) {
	this->copy(that);
}
Matrix Matrix::operator+ (const Matrix& that) const {
	assert(this->sz == that.sz);
	Matrix result(this->sz);
	for (int i = 0; i < this->sz; i++) {
		for (int j = 0; j < this->sz; j++) {
			result[i][j] = (*this)[i][j] + that[i][j];
		}
	}
	return result;
}
Matrix Matrix::operator* (const Matrix& that) const {
	assert(this->sz == that.sz);
	Matrix result(this->sz);
	for (int i = 0; i < this->sz; i++) {
		for (int j = 0; j < this->sz; j++) {
			int sum = 0;
			for (int k = 0; k < this->sz; k++) {
				sum += (*this)[i][k] * that[k][j];
			}
			result[i][j] = sum;
		}
	}
	return result;
}
Matrix& Matrix::operator= (const Matrix& that) {
	if (this != &that) {
		this->~Matrix();
		this->copy(that);
	}
	return *this;
}
Matrix& Matrix::trans() {
	for (int i = 0; i < this->sz; i++) {
		for (int j = 1 + i; j < this->sz; j++) {
			int c = (*this)[i][j];
			(*this)[i][j] = (*this)[j][i];
			(*this)[j][i] = c;
		}
	}
	return *this;
}
void Matrix::print(std::ofstream& fout) {
	for (int i = 0; i < this->sz; i++) {
		for (int j = 0; j < this->sz; j++) {
			fout << (*this)[i][j] << " ";
		}
		fout << std::endl;
	}
}
Matrix::~Matrix() {
	this->delete_arr();
}
void Matrix::Vector::Vector_init(const int n) {
	this->data = new int[n];
}
Matrix::Vector::Vector(const int n):size(n) {
	this->Vector_init(n);
}
Matrix::Vector::Vector(const Vector& that) {
	this->Vector_init(that.size);
	for (int i = 0; i < this->size; i++) {
		(*this)[i] = that[i];
	}
}
Matrix::Vector::~Vector() {
	delete[] this->data;
}

int main()
{
	srand(time(0));
	std::ifstream fin;
	fin.open("input.txt");
	int N, k;
	fin >> N >> k;
	Matrix A(N, fin), B(N, fin), C(N, fin), D(N, fin), K(N, k);
	Matrix res = (A + B * C.trans() + K) * D.trans();
	fin.close();
	std::ofstream fout;
	fout.open("output.txt");
	res.print(fout);
	fout.close();
	return 0;
}