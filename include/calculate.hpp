#pragma once

#include "tokens.hpp"
#include "Integral.hpp"
#include <unistd.h>
//#include <SFML/Graphics.hpp>


//void plot(const std::vector<FuncRunInfo> &functions, RunInfo &runInfo, bool costumStep) noexcept;
bool recCalculateCondition(const std::vector<FuncRunInfo> &functions, RunInfo &runInfo) noexcept;

template<class T>
T clamp(T min, T arg, T max) noexcept{ return arg<min ? min : arg>max ? max : arg; }



double recCalculate(const std::vector<FuncRunInfo> &functions, RunInfo &runInfo) noexcept{
Repeat:
	switch (functions[runInfo.funcIndex].code[runInfo.nodeIndex].type){
	case Token::Type::Value: ++runInfo.nodeIndex;
		return functions[runInfo.funcIndex].code[runInfo.nodeIndex-1].value;
	case Token::Type::LocalVariable: ++runInfo.nodeIndex;
		return runInfo.variables[runInfo.baseIndex + functions[runInfo.funcIndex].code[runInfo.nodeIndex-1].number];
	case Token::Type::Variable: ++runInfo.nodeIndex;
		return runInfo.variables[functions[runInfo.funcIndex].code[runInfo.nodeIndex-1].number];
	case Token::Type::LocalVariableDefinition:{
			const size_t varIndex = runInfo.nodeIndex;
			++runInfo.nodeIndex;
			runInfo.variables[runInfo.baseIndex + functions[runInfo.funcIndex].code[varIndex].number] = recCalculate(functions, runInfo);
		} goto Repeat;
	case Token::Type::VariableDefinition:{
			const size_t varIndex = runInfo.nodeIndex;
			++runInfo.nodeIndex;
			runInfo.variables[functions[runInfo.funcIndex].code[varIndex].number] = recCalculate(functions, runInfo);
		} goto Repeat;
	case Token::Type::Call:{
			const size_t funcIndex = functions[runInfo.funcIndex].code[runInfo.nodeIndex].numbers[0];
			const size_t argsCount = functions[runInfo.funcIndex].code[runInfo.nodeIndex].numbers[1];
			++runInfo.nodeIndex;

			const size_t prevBaseIndex = runInfo.baseIndex;
			const size_t newBaseIndex = std::size(runInfo.variables);

			runInfo.variables.resize(newBaseIndex + functions[funcIndex].varsCount);
			for (size_t i=0; i!=argsCount; ++i)
				runInfo.variables[newBaseIndex+i] = recCalculate(functions, runInfo);

			const size_t callerIndex = runInfo.funcIndex;
			const size_t returnNodeIndex = runInfo.nodeIndex;

			runInfo.funcIndex = funcIndex;
			runInfo.baseIndex = newBaseIndex;
			runInfo.nodeIndex = 0;
			
			const double result = recCalculate(functions, runInfo);

			runInfo.funcIndex = callerIndex;
			runInfo.baseIndex = prevBaseIndex;
			runInfo.nodeIndex = returnNodeIndex;

			return result;
		}

	case Token::Type::Conditional:{
		const size_t skip = functions[runInfo.funcIndex].code[runInfo.nodeIndex].number;
		++runInfo.nodeIndex;
		runInfo.nodeIndex += skip & (recCalculateCondition(functions, runInfo)-1);
		return recCalculate(functions, runInfo);
	}
	
	case Token::Type::Add:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			return x + recCalculate(functions, runInfo);
		}
	case Token::Type::Subtract:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			return x - recCalculate(functions, runInfo);
		}
	case Token::Type::Multiply:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			return x * recCalculate(functions, runInfo);
		}
	case Token::Type::Divide:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			const double y = recCalculate(functions, runInfo);
			if (y == 0.0) raiseError("division by zero");
			return x / y;
		}
	case Token::Type::Power:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			const double y = recCalculate(functions, runInfo);
			if (x<0.0 && fmod(y, 1.0)!=0.0) raiseError("negative base raised to non integral exponent");
			return pow(x, y);
		}
	case Token::Type::Abs: ++runInfo.nodeIndex;
		return abs(recCalculate(functions, runInfo));
	case Token::Type::UMinus: ++runInfo.nodeIndex;
		return -recCalculate(functions, runInfo);
	case Token::Type::Factorial: ++runInfo.nodeIndex;
		return tgamma(recCalculate(functions, runInfo) + 1.0);
	case Token::Type::Acos:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x<-1.0 || x>1.0) raiseError("argument is not in domain of acos");
			return acos(x);
		}
	case Token::Type::Acosh:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x < 0.0) raiseError("argument is not in domain of acosh");
			return acosh(x);
		}
	case Token::Type::Asin:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x<-1.0 || x>1.0) raiseError("argument is not in domain of asin");
			return asin(x);
		}
	case Token::Type::Asinh:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x < 0.0) raiseError("argument is not in domain of asinh");
			return asinh(x);
		}
	case Token::Type::Atan: ++runInfo.nodeIndex;
		return atan(recCalculate(functions, runInfo));
	case Token::Type::Atanh:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x<-1.0 || x>1.0) raiseError("argument is not in domain of atanh");
			return atanh(x);
		}
	case Token::Type::Acot: ++runInfo.nodeIndex;
		return M_PI_2 - atan(recCalculate(functions, runInfo));
	case Token::Type::Acoth:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x>-1.0 && x<1.0) raiseError("argument is not in domain of acoth");
			return log(2.0 / (x - 1.0) + 1.0) / 2.0;
		}
	case Token::Type::Atan2:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			return atan2(x, recCalculate(functions, runInfo));
		}
	case Token::Type::Cbrt: ++runInfo.nodeIndex;
		return cbrt(recCalculate(functions, runInfo));
	case Token::Type::Ceil: ++runInfo.nodeIndex;
		return ceil(recCalculate(functions, runInfo));
	case Token::Type::Cos: ++runInfo.nodeIndex;
		return cos(recCalculate(functions, runInfo));
	case Token::Type::Cosh: ++runInfo.nodeIndex;
		return cosh(recCalculate(functions, runInfo));
	case Token::Type::Erf: ++runInfo.nodeIndex;
		return erf(recCalculate(functions, runInfo));
	case Token::Type::Exp: ++runInfo.nodeIndex;
		return exp(recCalculate(functions, runInfo));
	case Token::Type::Exp2: ++runInfo.nodeIndex;
		return exp2(recCalculate(functions, runInfo));
	case Token::Type::Floor: ++runInfo.nodeIndex;
		return floor(recCalculate(functions, runInfo));
	case Token::Type::Gamma: ++runInfo.nodeIndex;
		return tgamma(recCalculate(functions, runInfo));
	case Token::Type::Ln:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x <= 0.0) raiseError("logarithm of non positive argument");
			return log(x);
		}
	case Token::Type::Log2:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x <= 0.0) raiseError("logarithm of non positive argument");
			return log2(x);
		}
	case Token::Type::Log10:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x <= 0.0) raiseError("logarithm of non positive argument");
			return log10(x);
		}
	case Token::Type::Log:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			const double y = recCalculate(functions, runInfo);
			if (y <= 0.0 || y == 1.0) raiseError("wrong logarithm's base");
			if (x == 0.0) raiseError("division by zero");
			return log(x) / log(y);
		}
	case Token::Type::Mod: { ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			const double y = recCalculate(functions, runInfo);
			if (y == 0.0) raiseError("modolo operation of base zero");
			return fmod(x, y);
		}
	case Token::Type::Product:{
			const size_t indexVarIndex = functions[runInfo.funcIndex].code[runInfo.nodeIndex].number;
			++runInfo.nodeIndex;
			const double first = recCalculate(functions, runInfo);
			const double last = recCalculate(functions, runInfo);
			if (fmod(first, 1.0)!=0.0 || fmod(last, 1.0)!=0.0) raiseError("product has non intergral boundaries");
			
			double product = 1.0;
			const size_t funcNodeIndex = runInfo.nodeIndex;
			for (double i=first; i<=last; i+=1.0){
				runInfo.variables[indexVarIndex] = i;
				runInfo.nodeIndex = funcNodeIndex;
				product *= recCalculate(functions, runInfo);
			}
			return product;
		}
	case Token::Type::Round: ++runInfo.nodeIndex;
		return round(recCalculate(functions, runInfo));
	case Token::Type::Sqrt:{ ++runInfo.nodeIndex;
			const double x = recCalculate(functions, runInfo);
			if (x < 0.0) raiseError("square roo of negative value");
			return sqrt(x);
		}
	case Token::Type::Step: ++runInfo.nodeIndex;
		return (double)!signbit(recCalculate(functions, runInfo));
	case Token::Type::Sin: ++runInfo.nodeIndex;
		return sin(recCalculate(functions, runInfo));
	case Token::Type::Sinh: ++runInfo.nodeIndex;
		return sinh(recCalculate(functions, runInfo));
	case Token::Type::Sum:{
			const size_t indexVarIndex = functions[runInfo.funcIndex].code[runInfo.nodeIndex].number;
			++runInfo.nodeIndex;
			const double first = recCalculate(functions, runInfo);
			const double last = recCalculate(functions, runInfo);
			if (fmod(first, 1.0)!=0.0 || fmod(last, 1.0)!=0.0) raiseError("sum has non intergral boundaries");
			
			double sum = 0.0;
			const size_t funcNodeIndex = runInfo.nodeIndex;
			for (double i=first; i<=last; i+=1.0){
				runInfo.variables[indexVarIndex] = i;
				runInfo.nodeIndex = funcNodeIndex;
				sum += recCalculate(functions, runInfo);
			}
			return sum;
		}
	case Token::Type::Sign: ++runInfo.nodeIndex;
		return sp::sign(recCalculate(functions, runInfo));
	case Token::Type::Tan: ++runInfo.nodeIndex;
		return tan(recCalculate(functions, runInfo));
	case Token::Type::Tanh: ++runInfo.nodeIndex;
		return tanh(recCalculate(functions, runInfo));
	case Token::Type::Cot: ++runInfo.nodeIndex;
		return  tan(M_PI_2 - recCalculate(functions, runInfo));
	case Token::Type::Coth: ++runInfo.nodeIndex;
		return 2.0 / (exp(2.0 * recCalculate(functions, runInfo)) - 1.0) + 1.0;
	case Token::Type::Integral:{
			const size_t indexVarIndex = functions[runInfo.funcIndex].code[runInfo.nodeIndex].number;
			++runInfo.nodeIndex;
			const double first = recCalculate(functions, runInfo);
			const double last = recCalculate(functions, runInfo);
			const double scale = (last - first) / 2.0;
			const double midpoint = (last + first) / 2.0;

			/* CALCULATIONS */{
				const size_t funcNodeIndex = runInfo.nodeIndex;

				double offset = sp::QuadratureNodes[0] * scale;
				runInfo.variables[indexVarIndex] = midpoint - offset;
				double funcResult = recCalculate(functions, runInfo);
				runInfo.variables[indexVarIndex] = midpoint + offset;

				runInfo.nodeIndex = funcNodeIndex;
				double sum = 0;
				for (size_t i=0; i!=32; ++i){
					offset = sp::QuadratureNodes[i] * scale;
					runInfo.variables[indexVarIndex] = midpoint - offset;
					runInfo.nodeIndex = funcNodeIndex;
					funcResult = recCalculate(functions, runInfo);
					runInfo.variables[indexVarIndex] = midpoint + offset;
					runInfo.nodeIndex = funcNodeIndex;
					sum += (funcResult + recCalculate(functions, runInfo)) * sp::QuadratureWeights[i];
				}
				return sum * scale;
			}
		}
		case Token::Type::Plot:
		//	plot(functions, runInfo, true);
			goto Repeat;
		case Token::Type::PlotAutoRes:
		//	plot(functions, runInfo, false);
			goto Repeat;
	default: raiseError("unexpected parsing error");
	}
}




bool recCalculateCondition(const std::vector<FuncRunInfo> &functions, RunInfo &runInfo) noexcept{
	switch (functions[runInfo.funcIndex].code[runInfo.nodeIndex].type){
	case Token::Type::Add:{ ++runInfo.nodeIndex;
		const bool x = recCalculateCondition(functions, runInfo);
		return x | recCalculateCondition(functions, runInfo);
	}
	case Token::Type::Multiply:{ ++runInfo.nodeIndex;
		const bool x = recCalculateCondition(functions, runInfo);
		return x & recCalculateCondition(functions, runInfo);
	}
	case Token::Type::UMinus: ++runInfo.nodeIndex;
		return !recCalculateCondition(functions, runInfo);
	
	case Token::Type::Equal:{ ++runInfo.nodeIndex;
		const double x = recCalculate(functions, runInfo);
		return x == recCalculate(functions, runInfo);
	}
	case Token::Type::NotEqual:{ ++runInfo.nodeIndex;
		const double x = recCalculate(functions, runInfo);
		return x != recCalculate(functions, runInfo);
	}
	case Token::Type::Lesser:{ ++runInfo.nodeIndex;
		const double x = recCalculate(functions, runInfo);
		return x < recCalculate(functions, runInfo);
	}
	case Token::Type::Greater:{ ++runInfo.nodeIndex;
		const double x = recCalculate(functions, runInfo);
		return x > recCalculate(functions, runInfo);
	}
	case Token::Type::LesserEqual:{ ++runInfo.nodeIndex;
		const double x = recCalculate(functions, runInfo);
		return x <= recCalculate(functions, runInfo);
	}
	case Token::Type::GreaterEqual:{ ++runInfo.nodeIndex;
		const double x = recCalculate(functions, runInfo);
		return x >= recCalculate(functions, runInfo);
	}
	
	default: raiseError("unexpected parsing error");
	}
}
									 
constexpr float axisPrecission(const double length){
	const double maxSteps = 10;

	double stepSize = length / maxSteps;

	const double stepSize1 = pow(2.0, floor(log2(stepSize)));

	return stepSize1;
}

									 
									 
/*
void plot(
	const std::vector<FuncRunInfo> &functions, RunInfo &runInfo, const bool costumSteps
) noexcept{
	sf::VideoMode videoMode{600, 600};
	ErrorEffect = Error::Ignore;

	const size_t indexVarIndex = functions[runInfo.funcIndex].code[runInfo.nodeIndex].number;
	++runInfo.nodeIndex;
	const double first = recCalculate(functions, runInfo);
	const double last = recCalculate(functions, runInfo);
	const double bottom = recCalculate(functions, runInfo);
	const double top = recCalculate(functions, runInfo);
	double steps = 1024.0;
	if (costumSteps) steps = recCalculate(functions, runInfo);
	const double step = (last - first) / steps;
	const size_t funcNodeIndex = runInfo.nodeIndex;
	
	double scaleX = 600.0 / (last - first);
	double scaleY = 600.0 / (top - bottom);

	float axisPosX = clamp(0.0, -first * scaleX, 600.0);
	float axisPosY = clamp(0.0, top * scaleY, 600.0);


	double axisStepValX = exp10(floor(log10(last - first)) - 1.0);
	float aixsStepX = axisStepValX * scaleX;
	double axisStepValY = exp10(floor(log10(top - bottom)) - 1.0);
	float aixsStepY = axisStepValY * scaleY;


	if (first>last || bottom>top) raiseError("wrong ranges in plot definition");
	if (step<=0.0) raiseError("step cannot be 0 or negative");
	
	{
		sf::RenderWindow window(videoMode, "plot", sf::Style::Close|sf::Style::Titlebar);
		sf::Font font;
		font.loadFromFile("/home/patryk/Programming/Programs/unispace.ttf");
		Draw:{
			runInfo.variables[indexVarIndex] = first;
			runInfo.nodeIndex = funcNodeIndex;
			
			sf::Vector2f curr;
			curr.x = 0.f;
			curr.y = axisPosY - recCalculate(functions, runInfo) * scaleY;

			for (double x=first+step; x<=last; x+=step){
				runInfo.variables[indexVarIndex] = x;
				runInfo.nodeIndex = funcNodeIndex;
			
				sf::Vector2f next;
				next.x = axisPosX + x * scaleX;
				next.y = axisPosY - recCalculate(functions, runInfo) * scaleY;

				sf::Vertex line[2] = {{curr}, {next}};
				window.draw(line, 2, sf::PrimitiveType::Lines);
				
				curr = next;
			}

			{
				sf::Vertex line[2] = {{sf::Vector2f{0.f, axisPosY}}, {sf::Vector2f{600.f, axisPosY}}};
				window.draw(line, 2, sf::PrimitiveType::Lines);
			}
			{
				sf::Vertex line[2] = {{sf::Vector2f{axisPosX, 0.f}}, {sf::Vector2f{axisPosX, 600.f}}};
				window.draw(line, 2, sf::PrimitiveType::Lines);
			}
			{
				sf::Text text;
				text.setFont(font);
				text.setColor(sf::Color{0, 200, 100});
				text.setScale(0.2f, 0.2f);
				char buffer[16];

				double axisVal = axisStepValX;
				for (float x=axisPosX; x<600.f; x+=aixsStepX){
					sf::Vertex line[2] = {{sf::Vector2f{x, axisPosY+5.f}}, {sf::Vector2f{x, axisPosY-5.f}}};
					window.draw(line, 2, sf::PrimitiveType::Lines);

					// text.setPosition(x, axisPosX-12.f);
					// const int chars = snprintf(buffer, 16, "%.4lf", axisVal);
					// text.m_string.m_string.resize(chars);
					// std::copy_n(buffer, chars, std::begin(text.m_string));
					// axisVal += axisStepValX;
					// window.draw(text);
				}
				axisVal = -axisStepValX;
				for (float x=axisPosX; x>0.f; x-=aixsStepX){
					sf::Vertex line[2] = {{sf::Vector2f{x, axisPosY+5.f}}, {sf::Vector2f{x, axisPosY-5.f}}};
					window.draw(line, 2, sf::PrimitiveType::Lines);

					// text.setPosition(x, axisPosX+6.f);
					// const int chars = snprintf(buffer, 16, "%.4lf", axisVal);
					// text.setString(buffer);
					// axisVal -= axisStepValX;
					// window.draw(text);
				}
				axisVal = axisStepValY;
				for (float y=axisPosX; y<600.f; y+=aixsStepY){
					sf::Vertex line[2] = {{sf::Vector2f{axisPosX+5.f, y}}, {sf::Vector2f{axisPosX-5.f, y}}};
					window.draw(line, 2, sf::PrimitiveType::Lines);

					// text.setPosition(axisPosY+6.f, y);
					// const int chars = snprintf(buffer, 16, "%.4lf", axisVal);
					// text.m_string.m_string.resize(chars);
					// text.setString(sf::String{buffer,});
					// axisVal += axisStepValY;
					// window.draw(text);
				}
				axisVal = -axisStepValX;
				for (float y=axisPosX; y>0.f; y-=aixsStepY){
					sf::Vertex line[2] = {{sf::Vector2f{axisPosX+5.f, y}}, {sf::Vector2f{axisPosX-5.f, y}}};
					window.draw(line, 2, sf::PrimitiveType::Lines);

					// text.setPosition(axisPosY-6.f, y);
					// const int chars = snprintf(buffer, 16, "%.4lf", axisVal);
					// text.m_string.m_string.resize(chars);
					// std::copy_n(buffer, chars, std::begin(text.m_string));
					// axisVal -= axisStepValY;
					// window.draw(text);
				}
			}
			window.display();
		}

		sf::Event event;
		while (true){
			while (window.pollEvent(event)){
				if (event.type == sf::Event::Closed){
					window.close();
					goto Return;
				}
				if (event.type==sf::Event::GainedFocus){
					goto Draw;
				}
			}
			usleep(100000);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && window.hasFocus()){
				window.close();
				goto Return;
			}
		}
	}
Return:
	ErrorEffect = Error::Exit;
}*/
