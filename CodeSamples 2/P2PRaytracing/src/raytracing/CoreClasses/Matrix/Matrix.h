#ifndef MATRIX_H
#define MATRIX_H

class Matrix
{
	public:
		double m[4][4];

		Matrix(void);
	
		Matrix(const Matrix &mat);
	
		~Matrix(void);

		Matrix operator=(const Matrix& rhs);
	
		Matrix operator * (const Matrix &mat) const;
	
		Matrix operator/ (const double d);

		Matrix set_identity(void);
};

#endif // !MATRIX_H
