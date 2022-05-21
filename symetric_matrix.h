#include "matrix.h"
template<typename T, int m>
class SymetricMatrix : public Matrix<T, m, m> {

public:
    explicit SymetricMatrix(T value = 0) : Matrix<T, m, m>(value) {}

    ~SymetricMatrix() = default;

    SymetricMatrix(const SymetricMatrix &toCopy);

    T relevant(int i, int j) const;

    T &operator()(int i, int j);

    T print(int i, int j) const;
};

template<typename T, int m>
SymetricMatrix<T, m>::SymetricMatrix(const SymetricMatrix &toCopy) {
    if (this->rows != toCopy.rows || this->columns != toCopy.columns) {
        Matrix<T, m, m>::Throw();
    }
    *this = toCopy;
}

template<typename T, int m>
T SymetricMatrix<T, m>::relevant(int i, int j) const {
    if (i <= j) {
        return Matrix<T, m, m>::relevant(i, j);
    } else {
        return Matrix<T, m, m>::relevant(j, i);
    }
}


template<typename T, int m>
T &SymetricMatrix<T, m>::operator()(int i, int j) {
    if (i <= j) {
        return Matrix<T, m, m>::operator()(i, j);
    } else {
        return Matrix<T, m, m>::operator()(j, i);
    }
}

template<typename T, int m>
T SymetricMatrix<T,m>::print(int i, int j) const {
    if(i<=j){
        return Matrix<T,m,m>::print(i,j);
    }
    else{
        return Matrix<T,m,m>::print(j,i);
    }
}