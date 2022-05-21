#ifndef MATRIX_MATRIX_H
#define MATRIX_MATRIX_H
#pragma once
#include <iosfwd>
#include <cstdio>
#include <bits/exception.h>
#include <vector>
#include <sstream>

/*Matrix object is generic class used for 2D arrays manage,having support in :
+, -, *, <<, [][], (i,j) operators.
Initialized in O(1) for any matrix size.
Supporting in Avg and Min functions which return average element value at the matrix and minimum value.
 */

template<typename T, int row, int column>
class Matrix {
    T *matrix;
    int *stackArray;
    int *connectArray;
    int top;
    int rows;
    int columns;
    T Default;

public:

    explicit Matrix(T value = 0);

    Matrix(Matrix &toCopy);

    ~Matrix();

    int GetRows() const { return rows; }

    int GetColumns() const { return columns; }

    T min() const;

    virtual T &operator()(int i, int j);

    Matrix<T, row, column> &operator=(Matrix<T, row, column> &copy);

    virtual T print(int i, int j) const;

    T avg() const;

    void Throw() const { throw IllegalOperation(); }

    virtual T relevant(int i, int j) const;

    class IllegalOperation : public std::exception {};

    Matrix<T, row, column> &operator+=(Matrix<T, row, column> &toAdd);

    friend std::ostream &operator<<(std::ostream &os, const Matrix<T, row, column> &matrix1) {
        for (int i = 0; i < row; i++) {
            std::vector<std::ostringstream> osss(column);
            for (int j = 0; j < column; j++) {
                T ToPrint = matrix1.print(i, j);
                osss[j] << ToPrint;
            }
            std::vector<std::string> strs(column);
            for (int k = 0; k < column; ++k)
                strs[k] = osss[k].str();
            std::vector<int> indices(column, 0);
            bool finished = false;
            while (!finished) {
                for (int m = 0; m < column; m++) {
                    for (; indices[m] < strs[m].size(); ++indices[m]) {
                        if (strs[m][indices[m]] == '\n') {
                            ++indices[m];
                            break;
                        } else {
                            os << strs[m][indices[m]];
                        }
                    }
                    if (m!=column-1)
                        os << ' ';
                }
                finished = true;
                for (int m = 0; m < column; m++) {
                    if (indices[m] < strs[m].size())
                        finished = false;
                }
                if (!finished)
                    os << '\n';
            }
            if(i<row-1)
                os<<'\n';
        }
        return os;
    }

    class CRow {
        friend class Matrix;

    public:
        T operator[](int col) {
            return parent.relevant(r, col);
        }

        CRow(Matrix &parent_, int row_) : parent(parent_), r(row_) {}

    private:
        Matrix &parent;
        int r;
    };

    CRow operator[](int r) {
        return CRow(*this, r);
    }
};


template<typename T, int row, int column>
T min(const Matrix<T, row, column> &m1) { return m1.min(); }

template<typename T, int row1, int column1, int row2, int column2>
Matrix<T, row1, column1> &operator+(Matrix<T, row1, column1> m1, Matrix<T, row2, column2> m2) {
    if (m1.GetRows() != m2.GetRows() || m1.GetColumns() != m2.GetColumns()) {
        m1.Throw();
    }
    Matrix<T, row1, column1> *mat = new Matrix<T, row1, column1>();
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < column1; j++) {
            (*mat)(i, j) = m1[i][j] + m2[i][j];
        }
    }
    return *mat;
}


template<typename T, int row1, int column1, int row2, int column2>
Matrix<T, row1, column2> &operator*(Matrix<T, row1, column1> m1, Matrix<T, row2, column2> m2) {
    if (m1.GetColumns() != m2.GetRows()) {
        m1.Throw();
    }
    Matrix<T, row1, column2> *mat = new Matrix<T, row1, column2>();
    T temp = 0;
    for (int i = 0; i < row1; i++) {
        for (int j = 0; j < column2; j++) {
            for (int r = 0; r < column1; r++) {
                temp += m1[i][r] * m2[r][j];
            }
            (*mat)(i, j) = temp;
            temp = 0;
        }
    }
    return *mat;
}

template<typename T, int row, int column>
Matrix<T, row, column> &operator*(Matrix<T, row, column> m1, int scalar) {
    Matrix<T, row, column> *mat = new Matrix<T, row, column>();
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            (*mat)(i, j) = m1[i][j] * scalar;
        }
    }
    return *mat;
}

template<typename T, int column, int row>
Matrix<T, row, column> &operator*(int scalar, Matrix<T, row, column> m1) {
    return m1 * scalar;
}

template<typename T, int row, int column, int row1, int column1>
bool operator==(Matrix<T, row, column> m1, Matrix<T, row1, column1> m2) {
    if (m1.GetRows() != m2.GetRows() || m1.GetColumns() != m2.GetColumns()) {
        return false;
    }
    bool ItIs = true;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (m1[i][j] != m2[i][j]) {
                ItIs = false;
                break;
            }
        }
    }
    return ItIs;
}

template<typename T, int column, int row>
bool operator!=(Matrix<T, row, column> m1, Matrix<T, row, column> m2) {
    return !(m1 == m2);
}


template<typename T, int row, int column>
Matrix<T, row, column>::Matrix(T value):matrix(new T[column * row]), stackArray(new int[column * row]),
                                        connectArray(new int[column * row]), top(0), rows(row), columns(column),
                                        Default(value) {}


template<typename T, int row, int column>
Matrix<T, row, column>::~Matrix() {
    delete[] matrix;
    delete[] stackArray;
    delete[] connectArray;
}

template<typename T, int row, int column>
Matrix<T, row, column>::Matrix(Matrix &toCopy):top(0), rows(row), columns(column) {
    if (this->rows != toCopy.rows || this->columns != toCopy.columns) {
        throw IllegalOperation();
    }
    *this = toCopy;
}

template<typename T, int row, int column>
T &Matrix<T, row, column>::operator()(int i, int j) {
    if (i >= this->rows || j >= this->columns || i < 0 || j < 0) {
        Throw();
    }
    if (connectArray[i * column + j] < top && connectArray[i * column + j] >= 0 &&
        stackArray[connectArray[i * column + j]] == i * column + j) {
        return matrix[i * column + j];
    } else {
        connectArray[i * column + j] = top;
        stackArray[top] = i * column + j;
        top++;
        return matrix[i * column + j];
    }
}

template<typename T, int row, int column>
T Matrix<T, row, column>::relevant(int i, int j) const {
    if (connectArray[i * column + j] < top && connectArray[i * column + j] >= 0 &&
        stackArray[connectArray[i * column + j]] == i * column + j) {
        return matrix[i * columns + j];
    } else {
        return Default;
    }
}

template<typename T, int row, int column>
Matrix<T, row, column> &Matrix<T, row, column>::operator=(Matrix<T, row, column> &copy) {
    if (this->rows != copy.rows || this->columns != copy.columns) {
        throw IllegalOperation();
    }
    if (this == &copy) {
        return *this;
    }
    this->rows = copy.rows;
    this->columns = copy.columns;
    this->Default = copy.Default;
    this->top = copy.top;
    this->connectArray = new int[this->rows * this->columns];
    this->stackArray = new int[this->rows * this->columns];
    this->matrix = new T[this->rows * this->columns];
    for (int i = 0; i < this->top; i++) {
        stackArray[i] = copy.stackArray[i];
        connectArray[stackArray[i]] = i;
        matrix[stackArray[i]] = copy.matrix[stackArray[i]];
    }
    return *this;
}


template<typename T, int row, int column>
T Matrix<T, row, column>::print(int i, int j) const {
    if (connectArray[i * column + j] < top && connectArray[i * column + j] >= 0 &&
        stackArray[connectArray[i * column + j]] == i * column + j) {
        return matrix[i * column + j];
    } else {
        return Default;
    }
}


template<typename T, int row, int column>
T Matrix<T, row, column>::avg() const {
    T sum = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (connectArray[i * column + j] < top && connectArray[i * column + j] >= 0 &&
                stackArray[connectArray[i * column + j]] == i * column + j) {
                sum += matrix[i * column + j];
            } else {
                sum += Default;
            }
        }
    }
    return sum / (row * column);
}

template<typename T, int row, int column>
Matrix<T, row, column> &Matrix<T, row, column>::operator+=(Matrix<T, row, column> &toAdd) {
    if (toAdd.GetRows() != this->rows || toAdd.GetColumns() != this->columns) {
        throw IllegalOperation();
    } else {
        T *mat = new T[row * column];
        int *connect = new int[row * column];
        int *stack = new int[row * column];
        int NTop = 0;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < column; j++) {
                if (connectArray[i * column + j] < top && connectArray[i * column + j] >= 0 &&
                    stackArray[connectArray[i * column + j]] == i * column + j) {
                    mat[i * column + j] = matrix[i * column + j] + toAdd[i][j];
                } else {
                    mat[i * column + j] = Default + toAdd[i][j];
                }
                connect[i * column + j] = NTop;
                stack[NTop] = i * columns + j;
                NTop++;
            }
        }
        this->top = NTop;
        delete[] stackArray;
        delete[] matrix;
        delete[] connectArray;
        this->matrix = mat;
        this->stackArray = stack;
        this->connectArray = connect;
        return *this;
    }
}

template<typename T, int row, int column>
T Matrix<T, row, column>::min() const {
    T min = Default;
    for (int i = 0; i < top; i++) {
        if (stackArray[i] >= 0 && stackArray[i] < rows * columns && connectArray[i] >= 0
            && connectArray[i] < columns * row && stackArray[connectArray[i]] == i) {
            if (matrix[stackArray[i]] < min) {
                min = matrix[stackArray[i]];
            }
        }
    }
    return min;
}



#endif //MATRIX_MATRIX_H