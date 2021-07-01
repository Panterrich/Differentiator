#ifndef CALCULATORH
#define CALCULATORH

//=====================================================================

const int MAX_WORD_SIZE = 40;

//======================================================================

double GetG(struct Equation* equation, char* line);

double GetD(struct Equation* equation);

double GetP(struct Equation* equation);

double GetE(struct Equation* equation);

double GetN(struct Equation* equation);

double GetT(struct Equation* equation);

double GetW(struct Equation* equation);

void Calc_require(struct Equation* equation, char symbol);
//=======================================================================

#endif