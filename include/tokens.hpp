#pragma once

#include "Utils.hpp"
#include "Containers.hpp"

#include <setjmp.h>

struct Token{
	union{
		std::array<char, 16> name;
		double value;
		size_t number;
		std::array<size_t, 2> numbers;
	};

	enum class Type : uint32_t{
		End, Value, LocalVariable, Variable, LocalVariableDefinition, VariableDefinition, FunctionDefinition, ArgsRef,
		ParenL, ParenR, BracketL, BracketR, BraceL, BraceR, Comma, Allocation, Call, Plot, PlotAutoRes,
		Assign,
		Add, Subtract, Divide, Multiply, Power, Abs,
		UPlus, UMinus, Factorial,
		Pi, Euler,
		Sqrt, Cbrt, Exp, Exp2, Ln, Log, Log2, Log10, Gamma, Erf,
		Mod, Floor, Ceil, Round, Sign, Step, Ramp,
		Sin, Cos, Tan, Cot, Sinh, Cosh, Tanh, Coth, Asin, Acos, Atan, Acot, Atan2, Asinh, Acosh, Atanh, Acoth,
		Sum, Product, Integral,
		Conditional, Equal, NotEqual, Lesser, Greater, LesserEqual, GreaterEqual
	} type;


	Token() noexcept {}
	Token(const Type initType) noexcept : type{initType} {}
	Token(const Type initType, const char *const s) noexcept : type{initType}{ strncpy(std::begin(this->name), s, 15); }
	Token(const Type initType, const double d) noexcept : value{d}, type{initType} {}
	Token(const Type initType, const size_t n) noexcept : number{n}, type{initType} {}
};


struct FuncRunInfo{
	std::vector<Token> code;
	size_t varsCount;
};

struct FuncCallInfo{
	std::array<char, 16> name;
	uint32_t argsCount;
};

struct FuncParseInfo{
	std::vector<FuncRunInfo> funcs;
	std::vector<FuncCallInfo> funcNames;
	std::vector<std::array<char, 16>> varNames;
	size_t funcIndex = 0;
};



struct RunInfo{
	size_t nodeIndex = 0;
	size_t baseIndex = 0;
	size_t funcIndex = 0;
	std::vector<double> variables;
};


jmp_buf jumpBuffer;

enum class Error{
	Exit, Ignore, Jump
};

Error ErrorEffect = Error::Exit;

void raiseError(const char *const s) noexcept{
	switch (ErrorEffect){
	case Error::Exit:
		fprintf(stderr, "ERROR: %s\n", s);
		exit(1);
	case Error::Ignore: break;
	case Error::Jump: longjmp(jumpBuffer, 1);
	}
}
void raiseError(const char *const s0, const char *const s1, const char *const s2) noexcept{
	switch (ErrorEffect){
	case Error::Exit:
		fprintf(stderr, "ERROR: %s%s%s\n", s0, s1, s2);
		exit(1);
	case Error::Ignore: break;
	case Error::Jump: longjmp(jumpBuffer, 1);
	}
}
bool isNotNameChar(const char x){ return (x<'a' || x>'z') && (x<'A' || x>'Z') && x!='_' && (x<'0' || x>'9'); }



std::vector<Token> makeTokens(const char *input) noexcept{
	std::vector<Token> tokens;
	tokens.reserve(32);
	while (*input){
		if ((*input>='0' && *input<='9') || *input=='.'){
			tokens.push_back(Token{Token::Type::Value, strtod(input, &const_cast<char *&>(input))});
		} else{
			switch (*input){
			case '#': ++input;
				while (*input!='\n' && *input!='\0') ++input;
				break;
			case '+': ++input;
				tokens.push_back(Token{Token::Type::Add});
				break;
			case '-': ++input;
				tokens.push_back(Token{Token::Type::Subtract});
				break;
			case '*': ++input;
				tokens.push_back(Token{Token::Type::Multiply});
				break;
			case '/': ++input;
				tokens.push_back(Token{Token::Type::Divide});
				break;
			case '^': ++input;
				tokens.push_back(Token{Token::Type::Power});
				break;
			case '|': ++input;
				tokens.push_back(Token{Token::Type::Abs});
				break;
			case '(': ++input;
				tokens.push_back(Token{Token::Type::ParenL});
				break;
			case ')': ++input;
				tokens.push_back(Token{Token::Type::ParenR});
				break;
			case '[': ++input;
				tokens.push_back(Token{Token::Type::BracketL});
				break;
			case ']': ++input;
				tokens.push_back(Token{Token::Type::BracketR});
				break;
			case '{': ++input;
				tokens.push_back(Token{Token::Type::BraceL});
				break;
			case '}': ++input;
				tokens.push_back(Token{Token::Type::BraceR});
				break;
			case '!': ++input;
				if (input[0] == '='){
					++input;
					tokens.push_back(Token{Token::Type::NotEqual});
					break;
				}
				tokens.push_back(Token{Token::Type::Factorial});
				break;
			case '<': ++input;
				if (input[0] == '='){
					++input;
					tokens.push_back(Token{Token::Type::LesserEqual});
					break;
				}
				tokens.push_back(Token{Token::Type::Lesser});
				break;
			case '>': ++input;
				if (input[0] == '='){
					++input;
					tokens.push_back(Token{Token::Type::GreaterEqual});
					break;
				}
				tokens.push_back(Token{Token::Type::Greater});
				break;
			case '=': ++input;
				if (input[0] == '='){
					++input;
					tokens.push_back(Token{Token::Type::Equal});
					break;
				}
				tokens.push_back(Token{Token::Type::Assign});
				break;
			case 'a':
				if (input[1]=='c' && input[2]=='o'){
					if (input[3] == 's'){
						if (input[4]=='h' && isNotNameChar(input[5])){
							tokens.push_back(Token{Token::Type::Acosh});
							input += 5; break;
						} else if (isNotNameChar(input[4])){
							tokens.push_back(Token{Token::Type::Acos});
							input += 4; break;
						}
					} else if (input[3] == 't'){
						if (input[4]=='h' && isNotNameChar(input[5])){
							tokens.push_back(Token{Token::Type::Acoth});
							input += 5; break;
						} else if (isNotNameChar(input[4])){
							tokens.push_back(Token{Token::Type::Acot});
							input += 4; break;
						}
					}
				} else if (input[1]=='s' && input[2]=='i' && input[3]=='n'){
					if(input[4]=='h' && isNotNameChar(input[5])){
						tokens.push_back(Token{Token::Type::Asinh});
						input += 5; break;
					} else if (isNotNameChar(input[4])){
						tokens.push_back(Token{Token::Type::Asin});
						input += 4; break;
					}
				} else if (input[1]=='t' && input[2]=='a' && input[3]=='n'){
					if (input[4]=='h' && isNotNameChar(input[5])){
						tokens.push_back(Token{Token::Type::Atanh});
						input += 5; break;
					} else if (input[4]=='2' && isNotNameChar(input[5])){
						tokens.push_back(Token{Token::Type::Atan2});
						input += 5; break;
					} else if (isNotNameChar(input[4])){
						tokens.push_back(Token{Token::Type::Atan});
						input += 4; break;
					}
				} goto Default;
			case 'c':
				if (input[1]=='e' && input[2]=='i' && input[3]=='l' && isNotNameChar(input[4])){
					tokens.push_back(Token{Token::Type::Ceil});
					input += 4; break;
				} else if (input[1]=='b' && input[2]=='r' && input[3]=='t' && isNotNameChar(input[4])){
					tokens.push_back(Token{Token::Type::Cbrt});
					input += 4; break;
				} else if (input[1]=='o'){
					if (input[2] == 's'){
						if (input[3]=='h' && isNotNameChar(input[4])){
							tokens.push_back(Token{Token::Type::Cosh});
							input += 4; break;
						} else if (isNotNameChar(input[3])){
							tokens.push_back(Token{Token::Type::Cos});
							input += 3; break;
						}
					} else if (input[2] == 't'){
						if (input[3]=='h' && isNotNameChar(input[4])){
							tokens.push_back(Token{Token::Type::Coth});
							input += 4; break;
						} else if (isNotNameChar(input[3])){
							tokens.push_back(Token{Token::Type::Cot});
							input += 3; break;
						}
					}
				} goto Default;
			case 'e':
				if (input[1]=='x' && input[2]=='p'){
					if (input[3]=='2' && isNotNameChar(input[4])){
						tokens.push_back(Token{Token::Type::Exp2});
						input += 4; break;
					} else if (isNotNameChar(input[3])){	
						tokens.push_back(Token{Token::Type::Exp});
						input += 3; break;
					}
				} else if (input[1]=='r' && input[2]=='f' && isNotNameChar(input[3])){
					tokens.push_back(Token{Token::Type::Erf});
					input += 3; break;
				} else if (isNotNameChar(input[1])){
					tokens.push_back(Token{Token::Type::Euler});
					input += 1; break;
				} goto Default;
			case 'f':
				if (input[1]=='l' && input[2]=='o' && input[3]=='o' && input[4]=='r' && isNotNameChar(input[5])){
					tokens.push_back(Token{Token::Type::Floor});
					input += 5; break;
				} goto Default;
			case 'g':
				if ((input[1]=='a' && input[2]=='m') && (input[2]=='m' && input[4]=='a' && isNotNameChar(input[5]))){
					tokens.push_back(Token{Token::Type::Gamma});
					input += 5; break;
				} goto Default;
			case 'i':
				if (input[1]=='n' && input[2]=='t' && input[3]=='['){
					tokens.push_back(Token{Token::Type::Integral});
					input += 4; break;
				} goto Default;
			case 'l':
				if (input[1]=='n' && isNotNameChar(input[4])){
					tokens.push_back(Token{Token::Type::Ln});
					input += 2; break;
				} else if (input[1]=='o' && input[2]=='g'){
					if(input[3]=='2' && isNotNameChar(input[4])){
						tokens.push_back(Token{Token::Type::Log2});
						input += 4; break;
					} else if (input[3]=='1' && input[4]=='0' && isNotNameChar(input[5])){
						tokens.push_back(Token{Token::Type::Log10});
						input += 5; break;
					} else if (isNotNameChar(input[3])){
						tokens.push_back(Token{Token::Type::Log});
						input += 3; break;
					}
				} goto Default;
				break;
			case 'm':
				if (input[1]=='o' && input[2]=='d'){
					tokens.push_back(Token{Token::Type::Mod});
					input += 3; break;
				} goto Default;
			case 'p':
				if (input[1]=='i' && isNotNameChar(input[2])){
					tokens.push_back(Token{Token::Type::Pi});
					input += 2; break;
				} else if (input[1]=='r' && input[2]=='o' && input[3]=='d' && input[4]=='['){
					tokens.push_back(Token{Token::Type::Product});
					input += 5; break;
				} else if (input[1]=='l' && input[2]=='o' && input[3]=='t' && input[4]=='['){
					tokens.push_back(Token{Token::Type::Plot});
					input += 5; break;
				} goto Default;
			case 'r':
				if (input[1]=='o' && input[2]=='u' && input[3]=='n' && input[4]=='d' && isNotNameChar(input[5])){
					tokens.push_back(Token{Token::Type::Round});
					input += 5; break;
				} else if (input[1]=='a' && input[2]=='m' && input[3]=='p' && isNotNameChar(input[4])){
					tokens.push_back(Token{Token::Type::Ramp});
					input += 3; break;
				} goto Default;
			case 's':
				if (input[1]=='u' && input[2]=='m' && input[3]=='['){
					tokens.push_back(Token{Token::Type::Sum});
					input += 4; break;
				} else if (input[1]=='q' && input[2]=='r' && input[3]=='t' && isNotNameChar(input[4])){
					tokens.push_back(Token{Token::Type::Sqrt});
					input += 4; break;
				} else if (input[1]=='t' && input[2]=='e' && input[3]=='p' && isNotNameChar(input[4])){
					tokens.push_back(Token{Token::Type::Step});
					input += 4; break;
				} else if (input[1]=='i'){
					if (input[2]=='n'){
						if (input[3]=='h' && isNotNameChar(input[4])){
							tokens.push_back(Token{Token::Type::Sinh});
							input += 4; break;
						} else if (isNotNameChar(input[3])){
							tokens.push_back(Token{Token::Type::Sin});
							input += 3; break;
						}
					} else if (input[2]=='g' && input[3]=='n' && isNotNameChar(input[4])){
						tokens.push_back(Token{Token::Type::Sign});
						input += 4; break;
					}
				} goto Default;
			case 't':
				if(input[1]=='a' && input[2]=='n'){
					if (input[3]=='h' && isNotNameChar(input[4])){
						tokens.push_back(Token{Token::Type::Tanh});
						input += 4; break;
					}
					else if (isNotNameChar(input[3])){
						tokens.push_back(Token{Token::Type::Tan});
						input += 3; break;
					}
				} goto Default;
			case ',': ++input;
				tokens.push_back(Token{Token::Type::Comma});
				break;
			case '\n':
			case '\t':
			case ' ': ++input;
				break;
			default:
			Default:
				if ((*input>='a' && *input<='z') || (*input>='A' && *input<='Z') || *input=='_'){
					tokens.push_back(Token{Token::Type::Variable});
					tokens.back().name[0] = *input;
					++input;
					int i = 1;
					for (; i<15 && !isNotNameChar(*input); ++i, ++input)
						tokens.back().name[i] = *input;
					tokens.back().name[i] = 0;
				} else goto Return;
			}
		}
	}
Return:
	tokens.push_back(Token{Token::Type::End});
	return tokens;
}
