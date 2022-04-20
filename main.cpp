#include <stdio.h>
#include "calculate.hpp"

std::vector<FuncRunInfo> parseTokens(const Token *input) noexcept;
double recCalculate(const std::vector<FuncRunInfo> &functions, RunInfo &runInfo) noexcept;

size_t parse0(const Token **tokenI, FuncParseInfo *parseInfo) noexcept;
size_t parse1(const Token **tokenI, FuncParseInfo *parseInfo) noexcept;
size_t parse2(const Token **tokenI, FuncParseInfo *parseInfo) noexcept;
size_t parseExpression(const Token **tokenI, FuncParseInfo *parseInfo) noexcept;
size_t parsePostfix(const Token **tokenI, FuncParseInfo *info, size_t rootNodeIndex) noexcept;

void parseFunction(const Token **tokenI, FuncParseInfo *parseInfo) noexcept;


int main(int argc, char **argv){
//	argc = 1;	
	std::vector<Token> tokens;

	if (argc > 3) raiseError("too many arguments for command \"calc\"");

	
	std::vector<FuncRunInfo> functions;
	std::vector<char> buffer;

	if (argc==3 && !strncmp(argv[1], "-f", 2)){

		const bool printFile = argv[1][2]=='p' ? true :
			[x=argv[1][2]](){ if (x){ puts("ERROR: wrong fomat argument"); exit(1); } else return false; }();


		FILE *const inputFile = fopen(argv[2], "r");
		if (!inputFile){
			puts("ERROR: this file does not exist");
			return 1;
		}
		fseek(inputFile, 0, SEEK_END);
   		const size_t fileSize = ftell(inputFile);

   		buffer.resize(fileSize + 1);

   		rewind(inputFile);
   		fread(&*std::begin(buffer), sizeof(char), fileSize, inputFile);
		
		if (printFile) printf("%s ", &*std::begin(buffer));
		fclose(inputFile);
		
		buffer[fileSize] = '\0';
		tokens = makeTokens(&*std::begin(buffer));
	} else if (argc == 1){
		setjmp(jumpBuffer);
		ErrorEffect = Error::Exit;
		fflush(stdin);
		for (;;){
			int c = getc(stdin);
			if (c == EOF){
				buffer.push_back('\0');
				tokens = makeTokens(&*std::begin(buffer));
				break;
			}
			
			if (c == '\n'){
				buffer.push_back('\0');

				ErrorEffect = Error::Jump;
				tokens = makeTokens(&*std::begin(buffer));
				buffer.back() = '\n';
				functions = parseTokens(&*std::begin(tokens));
				ErrorEffect = Error::Exit;
				
				goto Compute;
			}
			buffer.push_back((char)c);
		}
	} else tokens = makeTokens(argv[1]);

	functions = parseTokens(&*std::begin(tokens));

Compute:
	RunInfo runInfo;
	runInfo.variables.resize(functions.front().varsCount);

	printf("= %lf\n", recCalculate(functions, runInfo));
	
	return 0;
}








size_t parse0(const Token **const tokenI, FuncParseInfo *const info) noexcept{
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;
	const size_t currNodeIndex = std::size(*nodes);

	switch (iter->type){
	case Token::Type::Value:
		nodes->emplace_back(Token::Type::Value, iter->value);
		++iter;
		return parsePostfix(tokenI, info, currNodeIndex);
	case Token::Type::Variable:{
			const char *const varName = std::begin(iter->name);
			for (size_t i=std::size(info->varNames); i; --i)
				if (!strcmp(std::begin(info->varNames[i-1]), varName)){
					++iter;
					if (i > info->funcs.front().varsCount)
						nodes->emplace_back(Token::Type::LocalVariable, i-info->funcs.front().varsCount-1);
					else
						nodes->emplace_back(Token::Type::Variable, i-1);
					return parsePostfix(tokenI, info, std::size(*nodes)-1);
				}
			if (iter[1].type == Token::Type::ParenL){
				for (size_t i=0; i!=std::size(info->funcNames); ++i)
					if (!strcmp(std::begin(info->funcNames[i].name), varName)){
						iter += 2;
						size_t argsCount = info->funcNames[i].argsCount;
						
						nodes->emplace_back(Token::Type::Call);
						nodes->back().numbers[0] = i + 1;
						nodes->back().numbers[1] = argsCount;
						size_t exprSize = 1;

						size_t argCounter = 0;
						if (iter->type != Token::Type::ParenR){
							exprSize += parseExpression(tokenI, info);
							++argCounter;
							
							while (iter->type != Token::Type::ParenR){
								iter += iter->type == Token::Type::Comma;
								exprSize += parseExpression(tokenI, info);
								++argCounter;
							}
						} ++iter;
						if (argCounter != argsCount)
							raiseError("wrong number of arguments to function: ", varName, "");
						
						return parsePostfix(tokenI, info, std::size(*nodes)-exprSize);
					}
			}
			raiseError("variable \'", std::begin(iter->name), "\' was not declared");
			return 0;
		}
	case Token::Type::ParenL: ++iter;{
		parseExpression(tokenI, info);
		if (iter->type != Token::Type::ParenR)
			raiseError("pharenthesis were opened, but not closed");
		++iter;
		return parsePostfix(tokenI, info, currNodeIndex);
	}
	case Token::Type::Abs: ++iter;{
		nodes->emplace_back(Token::Type::Abs);
		parseExpression(tokenI, info);
		if (iter->type != Token::Type::Abs)
			raiseError("absolute value symbol was opened, but not closed");
		++iter;
		return parsePostfix(tokenI, info, currNodeIndex);
	}
	case Token::Type::BracketL:{ ++iter;
		const size_t nodeIndex = std::size(*nodes);
		nodes->emplace_back(Token::Type::Conditional);
		parseExpression(tokenI, info);
		if (iter->type != Token::Type::BracketR)
			raiseError("conditional brackets were opened but not closed");
		++iter;
		(*nodes)[nodeIndex].number = parseExpression(tokenI, info);
		iter += iter->type == Token::Type::Comma;
		parseExpression(tokenI, info);
		return std::size(*nodes) - nodeIndex;
	}
	case Token::Type::Add: ++iter;
		return parse0(tokenI, info);
	case Token::Type::Subtract: ++iter;
		nodes->emplace_back(Token::Type::UMinus);
		return 1 + parse0(tokenI, info);
	case Token::Type::Pi:
	case Token::Type::Euler:
		nodes->emplace_back(Token::Type::Value, iter->type==Token::Type::Pi ? M_PI : M_E);
		++iter;
		return parsePostfix(tokenI, info, currNodeIndex);
	case Token::Type::Acos:
	case Token::Type::Acosh:
	case Token::Type::Asin:
	case Token::Type::Asinh:
	case Token::Type::Atan:
	case Token::Type::Atanh:
	case Token::Type::Acot:
	case Token::Type::Acoth:
	case Token::Type::Cbrt:
	case Token::Type::Ceil:
	case Token::Type::Cos:
	case Token::Type::Cosh:
	case Token::Type::Erf:
	case Token::Type::Exp:
	case Token::Type::Exp2:
	case Token::Type::Floor:
	case Token::Type::Gamma:
	case Token::Type::Ln:
	case Token::Type::Log10:
	case Token::Type::Log2:
	case Token::Type::Round:
	case Token::Type::Sqrt:
	case Token::Type::Step:
	case Token::Type::Sin:
	case Token::Type::Sinh:
	case Token::Type::Sign:
	case Token::Type::Tan:
	case Token::Type::Tanh:
	case Token::Type::Cot:
	case Token::Type::Coth:
		nodes->emplace_back(iter->type);
		++iter;
		if (iter->type == Token::Type::ParenL){
			++iter;
			parseExpression(tokenI, info);
			if (iter->type != Token::Type::ParenR)
				raiseError("pharenthesis were opened, but not closed");
			++iter;
			return parsePostfix(tokenI, info, currNodeIndex);
		}
		return 1 + parse0(tokenI, info);
	case Token::Type::Atan2:
	case Token::Type::Log:
	case Token::Type::Mod:
		nodes->emplace_back(iter->type);
		++iter;
		if (iter->type == Token::Type::ParenL){
			++iter;
			parseExpression(tokenI, info);
			iter += iter->type == Token::Type::Comma;
			parseExpression(tokenI, info);
			if (iter->type != Token::Type::ParenR)
				raiseError("pharenthesis were opened, but not closed");
			++iter;
			return parsePostfix(tokenI, info, currNodeIndex);
		}
		{
			const size_t s = 1 + parse0(tokenI, info);
			return s + parse0(tokenI, info);
		}

	case Token::Type::Sum:
	case Token::Type::Product:
	case Token::Type::Integral:{
		const Token::Type operationType = iter->type;
		++iter;
		constexpr const char *const options[3] = {"sum", "product", "integral"};
		if (iter->type != Token::Type::Variable)
			raiseError(
				"expected variable definition inside ",
				options[(uint32_t)operationType-(uint32_t)Token::Type::Sum],
				" expression"
			);
		
		const char *const indexVarName = std::begin(iter->name);
		if (iter[1].type != Token::Type::Assign)
			raiseError(
				"non initialized variable inside ",
				options[(uint32_t)operationType-(uint32_t)Token::Type::Sum],
				"expressin"
			);

		iter += 2 + (iter->type == Token::Type::Comma);

		nodes->emplace_back(operationType, info->funcs[info->funcIndex].varsCount);
		++info->funcs[info->funcIndex].varsCount;
		size_t exprSize = 1 + parseExpression(tokenI, info);
		exprSize += parseExpression(tokenI, info);

		if (iter->type != Token::Type::BracketR)
			raiseError(
				"missing closing square bracket inside",
				options[(uint32_t)operationType-(uint32_t)Token::Type::Sum],
				"expression"
			);
		++iter;

		info->varNames.emplace_back();
		strcpy(std::begin(info->varNames.back()), indexVarName);
		exprSize += parseExpression(tokenI, info);
		info->varNames.back()[0] = '\0';
		return exprSize;
	}
	default:
		raiseError("wrong syntax");
		return 0;
	}
}


size_t parse1(const Token **const tokenI, FuncParseInfo *const info) noexcept{
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;

	size_t exprSize = parse0(tokenI, info);
	{
		Token::Type tokType = iter->type;
		while (
			(tokType>=Token::Type::Equal && tokType<=Token::Type::GreaterEqual)
			|| tokType==Token::Type::Power
		){
			++iter;
			nodes->emplace_back();
			std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
			new(&*nodes->end()-exprSize-1) Token{tokType};
			exprSize += 1 + parse0(tokenI, info);
			tokType = iter->type;
		}
	}
	return exprSize;
}

size_t parse2(const Token **const tokenI, FuncParseInfo *const info) noexcept{
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;

	size_t exprSize = parse1(tokenI, info);
	{
		Token::Type tokType = iter->type;
		while (tokType==Token::Type::Multiply || tokType==Token::Type::Divide){
			++iter;
			nodes->emplace_back();
			std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
			new(&*nodes->end()-exprSize-1) Token{tokType};
			exprSize += 1 + parse1(tokenI, info);
			tokType = iter->type;
		}
	} return exprSize;
}

size_t parseExpression(const Token **const tokenI, FuncParseInfo *const info) noexcept{
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;

	size_t exprSize = parse2(tokenI, info);
	{
		Token::Type tokType = iter->type;
		while (tokType==Token::Type::Add || tokType==Token::Type::Subtract){
			++iter;
			nodes->emplace_back();
			std::move_backward(nodes->end()-exprSize-1, nodes->end()-1, nodes->end());
			new(&*nodes->end()-exprSize-1) Token{tokType};
			exprSize += 1 + parse2(tokenI, info);
			tokType = iter->type;
		}
	} return exprSize;
}

size_t parsePostfix(
	const Token **const tokenI, FuncParseInfo *const info, const size_t rootIndex	
) noexcept{
	const Token *&iter = *tokenI;
	std::vector<Token> *const nodes = &info->funcs[info->funcIndex].code;
	while (iter->type == Token::Type::Factorial){
		++iter;
		nodes->emplace_back();
		std::move_backward(std::begin(*nodes)+rootIndex, std::end(*nodes)-1, std::end(*nodes));
		new(&*std::begin(*nodes)+rootIndex) Token{Token::Type::Factorial};
	}
	return std::size(*nodes) - rootIndex;
}



std::vector<FuncRunInfo> parseTokens(const Token *input) noexcept{	// VarValues should be array of pointers to variable values stored in nodes
	const Token *iter = input;
	FuncParseInfo funcInfo;
	funcInfo.funcs.emplace_back();

	for (;;){
		if (iter[0].type==Token::Type::Variable && iter[1].type==Token::Type::Assign){
			const char *const varName = std::begin(iter->name);
			iter += 2;
			if (iter->type == Token::Type::ParenL){
				++iter;
				funcInfo.funcNames.emplace_back();
				strcpy(std::begin(funcInfo.funcNames.back().name), varName);
				parseFunction(&iter, &funcInfo);
			} else{
				for (auto I=std::begin(funcInfo.varNames); I!=std::end(funcInfo.varNames); ++I)
					if (!strcmp(std::begin(*I), varName)) raiseError("redefinition of variable: ", varName, "");
				
				funcInfo.funcs.front().code.emplace_back(Token::Type::VariableDefinition, funcInfo.funcs.front().varsCount);
				++funcInfo.funcs.front().varsCount;
				funcInfo.varNames.emplace_back();
				strcpy(std::begin(funcInfo.varNames.back()), varName);
				
				parseExpression(&iter, &funcInfo);
				iter += iter->type == Token::Type::Comma;
			}
		} else if (iter->type == Token::Type::Plot){
			raiseError("plotting is not supported");

			++iter;
			if (iter->type != Token::Type::Variable)
				raiseError("expected variable definition as first plot argument");
			
			const char *const indexVarName = std::begin(iter->name);
			if (iter[1].type != Token::Type::Assign)
				raiseError("plot's argument that defines range is missing");

			iter += 2 + (iter->type == Token::Type::Comma);

			funcInfo.funcs.front().code.emplace_back(Token::Type::PlotAutoRes, funcInfo.funcs.front().varsCount);
			++funcInfo.funcs.front().varsCount;
			size_t exprSize = 1 + parseExpression(&iter, &funcInfo);
			iter += iter->type == Token::Type::Comma;
			exprSize += parseExpression(&iter, &funcInfo);
			iter += iter->type == Token::Type::Comma;
			exprSize += parseExpression(&iter, &funcInfo);
			iter += iter->type == Token::Type::Comma;
			exprSize += parseExpression(&iter, &funcInfo);

			if (iter->type != Token::Type::BracketR){
				funcInfo.funcs.front().code[std::size(funcInfo.funcs.front().code)-exprSize].type = Token::Type::Plot;
				iter += iter->type == Token::Type::Comma;
				exprSize += parseExpression(&iter, &funcInfo);
				if (iter->type != Token::Type::BracketR)
					raiseError("missing closing square bracket inside plot argument list");
			}
			++iter;

			funcInfo.varNames.emplace_back();
			strcpy(std::begin(funcInfo.varNames.back()), indexVarName);
			exprSize += parseExpression(&iter, &funcInfo);
			funcInfo.varNames.back()[0] = '\0';
		} else{
			if (iter->type == Token::Type::End) raiseError("no returning statement");
			parseExpression(&iter, &funcInfo);
			if (iter->type != Token::Type::End)
				raiseError("statement after returning expression in function");
			break;
		}
	}
	return std::move(funcInfo.funcs);
}

void parseFunction(const Token **const tokenI, FuncParseInfo *const info) noexcept{	// VarValues should be array of pointers to variable values stored in nodes
	const Token *&iter = *tokenI;


	size_t argsCount = 0;
	while (iter->type != Token::Type::ParenR){
		if (iter->type != Token::Type::Variable)
			raiseError(
				"missing argument's name inside definition of function: ",
				std::begin(info->funcNames.back().name), ""
			);
		for (auto I=std::begin(info->varNames)+info->funcs.front().varsCount; I!=std::end(info->varNames); ++I)
			if (!strcmp(std::begin(*I), std::begin(iter->name)))
				raiseError("duplicated function's argument ", std::begin(info->funcNames.back().name), "");
		info->varNames.emplace_back();
		strcpy(std::begin(info->varNames.back()), std::begin(iter->name));
		++argsCount;
		++iter;
	}

	if (iter[1].type != Token::Type::BraceL)
		raiseError(
			"missing opening brace in:", std::begin(info->funcNames.back().name),
			" function definition"
		);
	iter += 2;

	info->funcNames.back().argsCount = argsCount;
	info->funcIndex = std::size(info->funcs);
	info->funcs.emplace_back();
	info->funcs.back().varsCount = argsCount;
	
	for (;;){
		if (iter[0].type==Token::Type::Variable && iter[1].type==Token::Type::Assign){
			const char *const varName = std::begin(iter->name);
			iter += 2;

			if (iter->type == Token::Type::ParenL) raiseError("function defined inside another function");		
			for (auto I=std::begin(info->varNames)+info->funcs.front().varsCount; I!=std::end(info->varNames); ++I)
				if (!strcmp(std::begin(*I), varName)) raiseError("redefinition of variable ", varName, "");
			
			info->funcs.back().code.emplace_back(Token::Type::LocalVariableDefinition, info->funcs.back().varsCount);
			++info->funcs.back().varsCount;
			info->varNames.emplace_back();
			strcpy(std::begin(info->varNames.back()), varName);
			
			parseExpression(&iter, info);
			iter += iter->type == Token::Type::Comma;
		} else{
			if (iter->type==Token::Type::BraceR) raiseError("no returning statement in function: ", std::begin(info->funcNames.back().name), "");
			parseExpression(tokenI, info);
			if (iter->type != Token::Type::BraceR) raiseError("statement after returning expression in function:", std::begin(info->funcNames.back().name), "");
			break;
		}
	}
	++iter;
	info->funcIndex = 0;
	info->varNames.resize(info->funcs.front().varsCount);
}
