/*

	 _____  __ _____________ _______  ______ ___________
	/     \|  |  \____ \__  \\_  __ \/  ___// __ \_  __ \
   |  Y Y  \  |  /  |_> > __ \|  | \/\___ \\  ___/|  | \/
   |__|_|  /____/|   __(____  /__|  /____  >\___  >__|
		 \/      |__|       \/           \/     \/
   Copyright (C) 2023 Ingo Berg

	Redistribution and use in source and binary forms, with or without modification, are permitted
	provided that the following conditions are met:

	  * Redistributions of source code must retain the above copyright notice, this list of
		conditions and the following disclaimer.
	  * Redistributions in binary form must reproduce the above copyright notice, this list of
		conditions and the following disclaimer in the documentation and/or other materials provided
		with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
	OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "muParserTest.h"

#include <cstdio>
#include <cmath>
#include <limits>

using namespace std;

/** \file
	\brief This file contains the implementation of parser test cases.
*/

namespace mu
{
	namespace Test
	{
		int ParserTester::c_iCount = 0;

		//---------------------------------------------------------------------------------------------
		ParserTester::ParserTester()
			:m_vTestFun()
		{
			AddTest(&ParserTester::TestNames);
			AddTest(&ParserTester::TestSyntax);
			AddTest(&ParserTester::TestPostFix);
			AddTest(&ParserTester::TestInfixOprt);
			AddTest(&ParserTester::TestVarConst);
			AddTest(&ParserTester::TestMultiArg);
			AddTest(&ParserTester::TestExpression);
			AddTest(&ParserTester::TestIfThenElse);
			AddTest(&ParserTester::TestInterface);
			AddTest(&ParserTester::TestBinOprt);
			AddTest(&ParserTester::TestException);
			AddTest(&ParserTester::TestStrArg);
			AddTest(&ParserTester::TestBulkMode);
			AddTest(&ParserTester::TestOptimizer);
			AddTest(&ParserTester::TestLocalization);

			ParserTester::c_iCount = 0;
		}

		//---------------------------------------------------------------------------------------------
		int ParserTester::IsHexVal(const char_type* a_szExpr, int* a_iPos, value_type* a_fVal)
		{
			if (a_szExpr[1] == 0 || (a_szExpr[0] != '0' || a_szExpr[1] != 'x'))
				return 0;

			unsigned iVal(0);

			// New code based on streams for UNICODE compliance:
			stringstream_type::pos_type nPos(0);
			stringstream_type ss(a_szExpr + 2);
			ss >> std::hex >> iVal;
			nPos = ss.tellg();

			if (nPos == (stringstream_type::pos_type)0)
				return 1;

			*a_iPos += (int)(2 + nPos);
			*a_fVal = (value_type)iVal;
			return 1;
		}

		//---------------------------------------------------------------------------------------------
		int ParserTester::TestInterface()
		{
			int iStat = 0;
			mu::console() << _T("testing member functions...");

			// Test RemoveVar
			value_type afVal[3] = { 1,2,3 };
			Parser p;

			try
			{
				p.DefineVar(_T("a"), &afVal[0]);
				p.DefineVar(_T("b"), &afVal[1]);
				p.DefineVar(_T("c"), &afVal[2]);
				p.SetExpr(_T("a+b+c"));
				p.Eval();
			}
			catch (...)
			{
				iStat += 1;  // this is not supposed to happen
			}

			try
			{
				p.RemoveVar(_T("c"));
				p.Eval();
				iStat += 1;  // not supposed to reach this, nonexisting variable "c" deleted...
			}
			catch (...)
			{
				// failure is expected...
			}

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------------------------
		int ParserTester::TestOptimizer()
		{
			int iStat = 0;
			mu::console() << _T("testing optimizer...");

			// Test RemoveVar
			Parser p;
			try
			{
				// test for #93 (https://github.com/beltoforion/muparser/issues/93)
				// expected bytecode is:
				// VAL, FUN
				{
					p.DefineFun(_T("unoptimizable"), f1of1, false);
					p.SetExpr(_T("unoptimizable(1)"));
					p.Eval();

					auto& bc = p.GetByteCode();
					const SToken* tok = bc.GetBase();
					if (bc.GetSize() != 2 && tok[1].Cmd != cmFUNC)
					{
						mu::console() << _T("#93 an unoptimizable expression was optimized!") << endl;
						iStat += 1;
					}
				}

				{
					p.ClearFun();
					p.DefineFun(_T("unoptimizable"), f1of1, true);
					p.SetExpr(_T("unoptimizable(1)"));
					p.Eval();

					auto& bc = p.GetByteCode();
					const SToken* tok = bc.GetBase();
					if (bc.GetSize() != 1 && tok[0].Cmd != cmVAL)
					{
						mu::console() << _T("#93 optimizer error") << endl;
						iStat += 1;
					}
				}
			}
			catch (...)
			{
				iStat += 1;  // this is not supposed to happen
			}

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------------------------
		int ParserTester::TestStrArg()
		{
			int iStat = 0;
			mu::console() << _T("testing string arguments...");

			// from oss-fuzz: https://bugs.chromium.org/p/oss-fuzz/issues/detail?id=23410
			iStat += ThrowTest(_T(R"(6 - 6 ? 4 : "", ? 4 : "", ? 4 : "")"), ecUNEXPECTED_STR, true);
			// variations:
			iStat += ThrowTest(_T(R"(avg(0?4:(""),1))"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"(1 ? 4 : "")"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"(1 ? "" : 4)"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"(1 ? "" : "")"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"(0 ? 4 : "")"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"(0 ? 4 : (""))"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"(1 ? 4 : "")"), ecUNEXPECTED_STR);

			// from oss-fuzz: https://oss-fuzz.com/testcase-detail/5106868061208576
			iStat += ThrowTest(_T(R"("","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",8)"), ecSTR_RESULT);

			// derived from oss-fuzz: https://oss-fuzz.com/testcase-detail/5758791700971520
			iStat += ThrowTest(_T("(\"\"), 7"), ecSTR_RESULT);
			iStat += ThrowTest(_T("((\"\")), 7"), ecSTR_RESULT);
			//iStat += ThrowTest(_T("(\"\"),(\" \"), 7, (3)"), ecSTR_RESULT);
			//iStat += ThrowTest(_T("(\"\"),(\"\"), 7, (3)"), ecSTR_RESULT);
			//iStat += ThrowTest(_T("(\"\"),(\"\"), (3)"), ecSTR_RESULT);
			//iStat += ThrowTest(_T("(\"\"),(\"\"), 7"), ecSTR_RESULT);


			// variations:
			iStat += ThrowTest(_T(R"("","",9)"), ecSTR_RESULT);

			iStat += EqnTest(_T("valueof(\"\")"), 123, true);   // empty string arguments caused a crash
			iStat += EqnTest(_T("valueof(\"aaa\")+valueof(\"bbb\")  "), 246, true);
			iStat += EqnTest(_T("2*(valueof(\"aaa\")-23)+valueof(\"bbb\")"), 323, true);

			// use in expressions with variables
			iStat += EqnTest(_T("a*(atof(\"10\")-b)"), 8, true);
			iStat += EqnTest(_T("a-(atof(\"10\")*b)"), -19, true);

			// string + numeric arguments
			iStat += EqnTest(_T("strfun1(\"100\")"), 100, true);
			iStat += EqnTest(_T("strfun2(\"100\",1)"), 101, true);
			iStat += EqnTest(_T("strfun3(\"99\",1,2)"), 102, true);
			iStat += EqnTest(_T("strfun4(\"99\",1,2,3)"), 105, true);
			iStat += EqnTest(_T("strfun5(\"99\",1,2,3,4)"), 109, true);
			iStat += EqnTest(_T("strfun6(\"99\",1,2,3,4,5)"), 114, true);

			// string constants
			iStat += EqnTest(_T("atof(str1)+atof(str2)"), 3.33, true);

			// user data
			iStat += EqnTest(_T("strfunud3_10(\"99\",1,2)"), 112, true);

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------------------------
		int ParserTester::TestBulkMode()
		{
			int iStat = 0;
			mu::console() << _T("testing bulkmode...");

#define EQN_TEST_BULK(EXPR, R1, R2, R3, R4, PASS) \
			{ \
			  double res[] = { R1, R2, R3, R4 }; \
			  iStat += EqnTestBulk(_T(EXPR), res, (PASS)); \
			}

			// Bulk Variables for the test:
			// a: 1,2,3,4
			// b: 2,2,2,2
			// c: 3,3,3,3
			// d: 5,4,3,2
			EQN_TEST_BULK("a", 1, 1, 1, 1, false)
			EQN_TEST_BULK("a", 1, 2, 3, 4, true)
			EQN_TEST_BULK("b=a", 1, 2, 3, 4, true)
			EQN_TEST_BULK("b=a, b*10", 10, 20, 30, 40, true)
			EQN_TEST_BULK("b=a, b*10, a", 1, 2, 3, 4, true)
			EQN_TEST_BULK("a+b", 3, 4, 5, 6, true)
			EQN_TEST_BULK("c*(a+b)", 9, 12, 15, 18, true)
#undef EQN_TEST_BULK

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------------------------
		int ParserTester::TestBinOprt()
		{
			int iStat = 0;
			mu::console() << _T("testing binary operators...");

			// built in operators
			// xor operator

			iStat += EqnTest(_T("a++b"), 3, true);
			iStat += EqnTest(_T("a ++ b"), 3, true);
			iStat += EqnTest(_T("1++2"), 3, true);
			iStat += EqnTest(_T("1 ++ 2"), 3, true);
			iStat += EqnTest(_T("a add b"), 3, true);
			iStat += EqnTest(_T("1 add 2"), 3, true);
			iStat += EqnTest(_T("a<b"), 1, true);
			iStat += EqnTest(_T("b>a"), 1, true);
			iStat += EqnTest(_T("a>a"), 0, true);
			iStat += EqnTest(_T("a<a"), 0, true);
			iStat += EqnTest(_T("a>a"), 0, true);
			iStat += EqnTest(_T("a<=a"), 1, true);
			iStat += EqnTest(_T("a<=b"), 1, true);
			iStat += EqnTest(_T("b<=a"), 0, true);
			iStat += EqnTest(_T("a>=a"), 1, true);
			iStat += EqnTest(_T("b>=a"), 1, true);
			iStat += EqnTest(_T("a>=b"), 0, true);

			// Test logical operators, especially if user defined "&" and the internal "&&" collide
			iStat += EqnTest(_T("1 && 1"), 1, true);
			iStat += EqnTest(_T("1 && 0"), 0, true);
			iStat += EqnTest(_T("(a<b) && (b>a)"), 1, true);
			iStat += EqnTest(_T("(a<b) && (a>b)"), 0, true);
			iStat += EqnTest(_T("12 & 255"), 12, true);
			iStat += EqnTest(_T("12 & 0"), 0, true);
			iStat += EqnTest(_T("12&255"), 12, true);
			iStat += EqnTest(_T("12&0"), 0, true);
			// test precedence of logic operators (should be like c++)
			iStat += EqnTest(_T("0 && 0 || 1"), 1, true);
			iStat += EqnTest(_T("0 && 1 || 0"), 0, true);
			iStat += EqnTest(_T("1 && 0 || 0"), 0, true);
			iStat += EqnTest(_T("1 && 1 || 0"), 1, true);
			iStat += EqnTest(_T("1 && 0 + 1"), 1, true);
			iStat += EqnTest(_T("1 && 1 - 1"), 0, true);

			// Assignment operator
			iStat += EqnTest(_T("a = b"), 2, true);
			iStat += EqnTest(_T("a = sin(b)"), 0.909297, true);
			iStat += EqnTest(_T("a = 1+sin(b)"), 1.909297, true);
			iStat += EqnTest(_T("(a=b)*2"), 4, true);
			iStat += EqnTest(_T("2*(a=b)"), 4, true);
			iStat += EqnTest(_T("2*(a=b+1)"), 6, true);
			iStat += EqnTest(_T("(a=b+1)*2"), 6, true);
			iStat += EqnTest(_T("a=c, a*10"), 30, true);

			iStat += EqnTest(_T("2^2^3"), 256, true);
			iStat += EqnTest(_T("1/2/3"), 1.0 / 6.0, true);

			// reference: http://www.wolframalpha.com/input/?i=3%2B4*2%2F%281-5%29^2^3
			iStat += EqnTest(_T("3+4*2/(1-5)^2^3"), 3.0001220703125, true);

			// Test user defined binary operators
			iStat += EqnTestInt(_T("1 | 2"), 3, true);
			iStat += EqnTestInt(_T("1 || 2"), 1, true);
			iStat += EqnTestInt(_T("123 & 456"), 72, true);
			iStat += EqnTestInt(_T("(123 & 456) % 10"), 2, true);
			iStat += EqnTestInt(_T("1 && 0"), 0, true);
			iStat += EqnTestInt(_T("123 && 456"), 1, true);
			iStat += EqnTestInt(_T("1 << 3"), 8, true);
			iStat += EqnTestInt(_T("8 >> 3"), 1, true);
			iStat += EqnTestInt(_T("9 / 4"), 2, true);
			iStat += EqnTestInt(_T("9 % 4"), 1, true);
			iStat += EqnTestInt(_T("if(5%2,1,0)"), 1, true);
			iStat += EqnTestInt(_T("if(4%2,1,0)"), 0, true);
			iStat += EqnTestInt(_T("-10+1"), -9, true);
			iStat += EqnTestInt(_T("1+2*3"), 7, true);
			iStat += EqnTestInt(_T("const1 != const2"), 1, true);
			iStat += EqnTestInt(_T("const1 != const2"), 0, false);
			iStat += EqnTestInt(_T("const1 == const2"), 0, true);
			iStat += EqnTestInt(_T("const1 == 1"), 1, true);
			iStat += EqnTestInt(_T("10*(const1 == 1)"), 10, true);
			iStat += EqnTestInt(_T("2*(const1 | const2)"), 6, true);
			iStat += EqnTestInt(_T("2*(const1 | const2)"), 7, false);
			iStat += EqnTestInt(_T("const1 < const2"), 1, true);
			iStat += EqnTestInt(_T("const2 > const1"), 1, true);
			iStat += EqnTestInt(_T("const1 <= 1"), 1, true);
			iStat += EqnTestInt(_T("const2 >= 2"), 1, true);
			iStat += EqnTestInt(_T("2*(const1 + const2)"), 6, true);
			iStat += EqnTestInt(_T("2*(const1 - const2)"), -2, true);
			iStat += EqnTestInt(_T("a != b"), 1, true);
			iStat += EqnTestInt(_T("a != b"), 0, false);
			iStat += EqnTestInt(_T("a == b"), 0, true);
			iStat += EqnTestInt(_T("a == 1"), 1, true);
			iStat += EqnTestInt(_T("10*(a == 1)"), 10, true);
			iStat += EqnTestInt(_T("2*(a | b)"), 6, true);
			iStat += EqnTestInt(_T("2*(a | b)"), 7, false);
			iStat += EqnTestInt(_T("a < b"), 1, true);
			iStat += EqnTestInt(_T("b > a"), 1, true);
			iStat += EqnTestInt(_T("a <= 1"), 1, true);
			iStat += EqnTestInt(_T("b >= 2"), 1, true);
			iStat += EqnTestInt(_T("2*(a + b)"), 6, true);
			iStat += EqnTestInt(_T("2*(a - b)"), -2, true);
			iStat += EqnTestInt(_T("a + (a << b)"), 5, true);
			iStat += EqnTestInt(_T("-2^2"), -4, true);
			iStat += EqnTestInt(_T("3--a"), 4, true);
			iStat += EqnTestInt(_T("3+-3^2"), -6, true);

			// Test reading of hex values:
			iStat += EqnTestInt(_T("0xff"), 255, true);
			iStat += EqnTestInt(_T("10+0xff"), 265, true);
			iStat += EqnTestInt(_T("0xff+10"), 265, true);
			iStat += EqnTestInt(_T("10*0xff"), 2550, true);
			iStat += EqnTestInt(_T("0xff*10"), 2550, true);
			iStat += EqnTestInt(_T("10+0xff+1"), 266, true);
			iStat += EqnTestInt(_T("1+0xff+10"), 266, true);

			// incorrect: '^' is yor here, not power
			//    iStat += EqnTestInt("-(1+2)^2", -9, true);
			//    iStat += EqnTestInt("-1^3", -1, true);

				  // Test precedence
				  // a=1, b=2, c=3
			iStat += EqnTestInt(_T("a + b * c"), 7, true);
			iStat += EqnTestInt(_T("a * b + c"), 5, true);
			iStat += EqnTestInt(_T("a<b && b>10"), 0, true);
			iStat += EqnTestInt(_T("a<b && b<10"), 1, true);

			iStat += EqnTestInt(_T("a + b << c"), 17, true);
			iStat += EqnTestInt(_T("a << b + c"), 7, true);
			iStat += EqnTestInt(_T("c * b < a"), 0, true);
			iStat += EqnTestInt(_T("c * b == 6 * a"), 1, true);
			iStat += EqnTestInt(_T("2^2^3"), 256, true);


			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------------------------
		/** \brief Check muParser name restriction enforcement. */
		int ParserTester::TestNames()
		{
			int  iStat = 0,
				iErr = 0;

			mu::console() << "testing name restriction enforcement...";

			Parser p;

#define PARSER_THROWCHECK(DOMAIN, FAIL, EXPR, ARG)	\
			iErr = 0;										\
			ParserTester::c_iCount++;						\
			try												\
			{												\
				p.Define##DOMAIN(EXPR, ARG);                \
				iErr = (FAIL) ? 0 : 1;						\
			}												\
			catch(...)										\
			{												\
				iErr = (!FAIL) ? 0 : 1;						\
			}												\
			iStat += iErr;

			// constant names
			PARSER_THROWCHECK(Const, false, _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), 1)
			PARSER_THROWCHECK(Const, false, _T("0a"), 1)
			PARSER_THROWCHECK(Const, false, _T("9a"), 1)
			PARSER_THROWCHECK(Const, false, _T("+a"), 1)
			PARSER_THROWCHECK(Const, false, _T("-a"), 1)
			PARSER_THROWCHECK(Const, false, _T("a-"), 1)
			PARSER_THROWCHECK(Const, false, _T("a*"), 1)
			PARSER_THROWCHECK(Const, false, _T("a?"), 1)
			PARSER_THROWCHECK(Const, true, _T("a"), 1)
			PARSER_THROWCHECK(Const, true, _T("a_min"), 1)
			PARSER_THROWCHECK(Const, true, _T("a_min0"), 1)
			PARSER_THROWCHECK(Const, true, _T("a_min9"), 1)

				// variable names
				value_type a;
			p.ClearConst();
			PARSER_THROWCHECK(Var, false, _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), &a);
			PARSER_THROWCHECK(Var, false, _T("123abc"), &a)
			PARSER_THROWCHECK(Var, false, _T("9a"), &a)
			PARSER_THROWCHECK(Var, false, _T("0a"), &a)
			PARSER_THROWCHECK(Var, false, _T("+a"), &a)
			PARSER_THROWCHECK(Var, false, _T("-a"), &a)
			PARSER_THROWCHECK(Var, false, _T("?a"), &a)
			PARSER_THROWCHECK(Var, false, _T("!a"), &a)
			PARSER_THROWCHECK(Var, false, _T("a+"), &a)
			PARSER_THROWCHECK(Var, false, _T("a-"), &a)
			PARSER_THROWCHECK(Var, false, _T("a*"), &a)
			PARSER_THROWCHECK(Var, false, _T("a?"), &a)
			PARSER_THROWCHECK(Var, true, _T("a"), &a)
			PARSER_THROWCHECK(Var, true, _T("a_min"), &a)
			PARSER_THROWCHECK(Var, true, _T("a_min0"), &a)
			PARSER_THROWCHECK(Var, true, _T("a_min9"), &a)
			PARSER_THROWCHECK(Var, false, _T("a_min9"), 0)

			// Postfix operators
			// fail
			PARSER_THROWCHECK(PostfixOprt, false, _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), f1of1);
			PARSER_THROWCHECK(PostfixOprt, false, _T("(k"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, false, _T("9+"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, false, _T("+"), 0)
			// pass
			PARSER_THROWCHECK(PostfixOprt, true, _T("-a"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("?a"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("_"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("#"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("&&"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("||"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("&"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("|"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("++"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("--"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("?>"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("?<"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("**"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("xor"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("and"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("or"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("not"), f1of1)
			PARSER_THROWCHECK(PostfixOprt, true, _T("!"), f1of1)

			// Binary operator
			// The following must fail with builtin operators activated
			// p.EnableBuiltInOp(true); -> this is the default
			p.ClearPostfixOprt();
			PARSER_THROWCHECK(Oprt, false, _T("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), f1of2);
			PARSER_THROWCHECK(Oprt, false, _T("+"), f1of2)
			PARSER_THROWCHECK(Oprt, false, _T("-"), f1of2)
			PARSER_THROWCHECK(Oprt, false, _T("*"), f1of2)
			PARSER_THROWCHECK(Oprt, false, _T("/"), f1of2)
			PARSER_THROWCHECK(Oprt, false, _T("^"), f1of2)
			PARSER_THROWCHECK(Oprt, false, _T("&&"), f1of2)
			PARSER_THROWCHECK(Oprt, false, _T("||"), f1of2)

			// without activated built in operators it should work
			p.EnableBuiltInOprt(false);
			PARSER_THROWCHECK(Oprt, true, _T("+"), f1of2)
			PARSER_THROWCHECK(Oprt, true, _T("-"), f1of2)
			PARSER_THROWCHECK(Oprt, true, _T("*"), f1of2)
			PARSER_THROWCHECK(Oprt, true, _T("/"), f1of2)
			PARSER_THROWCHECK(Oprt, true, _T("^"), f1of2)
			PARSER_THROWCHECK(Oprt, true, _T("&&"), f1of2)
			PARSER_THROWCHECK(Oprt, true, _T("||"), f1of2)
#undef PARSER_THROWCHECK

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------
		int ParserTester::TestSyntax()
		{
			int iStat = 0;
			mu::console() << _T("testing syntax engine...");

			iStat += ThrowTest(_T("1,"), ecUNEXPECTED_EOF);  // incomplete hex definition
			iStat += ThrowTest(_T("a,"), ecUNEXPECTED_EOF);  // incomplete hex definition
			iStat += ThrowTest(_T("sin(8),"), ecUNEXPECTED_EOF);  // incomplete hex definition
			iStat += ThrowTest(_T("(sin(8)),"), ecUNEXPECTED_EOF);  // incomplete hex definition
			iStat += ThrowTest(_T("a{m},"), ecUNEXPECTED_EOF);  // incomplete hex definition

			iStat += EqnTest(_T("(1+ 2*a)"), 3, true);   // Spaces within formula
			iStat += EqnTest(_T("sqrt((4))"), 2, true);  // Multiple brackets
			iStat += EqnTest(_T("sqrt((2)+2)"), 2, true);// Multiple brackets
			iStat += EqnTest(_T("sqrt(2+(2))"), 2, true);// Multiple brackets
			iStat += EqnTest(_T("sqrt(a+(3))"), 2, true);// Multiple brackets
			iStat += EqnTest(_T("sqrt((3)+a)"), 2, true);// Multiple brackets
			iStat += EqnTest(_T("order(1,2)"), 1, true); // May not cause name collision with operator "or"
			iStat += EqnTest(_T("(2+"), 0, false);       // missing closing bracket
			iStat += EqnTest(_T("2++4"), 0, false);      // unexpected operator
			iStat += EqnTest(_T("2+-4"), 0, false);      // unexpected operator
			iStat += EqnTest(_T("(2+)"), 0, false);      // unexpected closing bracket
			iStat += EqnTest(_T("--2"), 0, false);       // double sign
			iStat += EqnTest(_T("ksdfj"), 0, false);     // unknown token
			iStat += EqnTest(_T("()"), 0, false);        // empty bracket without a function
			iStat += EqnTest(_T("5+()"), 0, false);      // empty bracket without a function
			iStat += EqnTest(_T("sin(cos)"), 0, false);  // unexpected function
			iStat += EqnTest(_T("5t6"), 0, false);       // unknown token
			iStat += EqnTest(_T("5 t 6"), 0, false);     // unknown token
			iStat += EqnTest(_T("8*"), 0, false);        // unexpected end of formula
			iStat += EqnTest(_T(",3"), 0, false);        // unexpected comma
			iStat += EqnTest(_T("3,5"), 0, false);       // unexpected comma
			iStat += EqnTest(_T("sin(8,8)"), 0, false);  // too many function args
			iStat += EqnTest(_T("(7,8)"), 0, false);     // too many function args
			iStat += EqnTest(_T("sin)"), 0, false);      // unexpected closing bracket
			iStat += EqnTest(_T("a)"), 0, false);        // unexpected closing bracket
			iStat += EqnTest(_T("pi)"), 0, false);       // unexpected closing bracket
			iStat += EqnTest(_T("sin(())"), 0, false);   // unexpected closing bracket
			iStat += EqnTest(_T("sin()"), 0, false);     // unexpected closing bracket

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------
		int ParserTester::TestVarConst()
		{
			int iStat = 0;
			mu::console() << _T("testing variable/constant detection...");

			// Test if the result changes when a variable changes
			iStat += EqnTestWithVarChange(_T("a"), 1, 1, 2, 2);
			iStat += EqnTestWithVarChange(_T("2*a"), 2, 4, 3, 6);

			// distinguish constants with same basename
			iStat += EqnTest(_T("const"), 1, true);
			iStat += EqnTest(_T("const1"), 2, true);
			iStat += EqnTest(_T("const2"), 3, true);
			iStat += EqnTest(_T("2*const"), 2, true);
			iStat += EqnTest(_T("2*const1"), 4, true);
			iStat += EqnTest(_T("2*const2"), 6, true);
			iStat += EqnTest(_T("2*const+1"), 3, true);
			iStat += EqnTest(_T("2*const1+1"), 5, true);
			iStat += EqnTest(_T("2*const2+1"), 7, true);
			iStat += EqnTest(_T("const"), 0, false);
			iStat += EqnTest(_T("const1"), 0, false);
			iStat += EqnTest(_T("const2"), 0, false);

			// distinguish variables with same basename
			iStat += EqnTest(_T("a"), 1, true);
			iStat += EqnTest(_T("aa"), 2, true);
			iStat += EqnTest(_T("2*a"), 2, true);
			iStat += EqnTest(_T("2*aa"), 4, true);
			iStat += EqnTest(_T("2*a-1"), 1, true);
			iStat += EqnTest(_T("2*aa-1"), 3, true);

			// custom value recognition
			iStat += EqnTest(_T("0xff"), 255, true);
			iStat += EqnTest(_T("0x97 + 0xff"), 406, true);

			// Finally test querying of used variables
			try
			{
				int idx;
				mu::Parser p;
				mu::value_type vVarVal[] = { 1, 2, 3, 4, 5 };
				p.DefineVar(_T("a"), &vVarVal[0]);
				p.DefineVar(_T("b"), &vVarVal[1]);
				p.DefineVar(_T("c"), &vVarVal[2]);
				p.DefineVar(_T("d"), &vVarVal[3]);
				p.DefineVar(_T("e"), &vVarVal[4]);

				// Test lookup of defined variables
				// 4 used variables
				p.SetExpr(_T("a+b+c+d"));
				mu::varmap_type UsedVar = p.GetUsedVar();
				int iCount = (int)UsedVar.size();
				if (iCount != 4)
					throw false;

				// the next check will fail if the parser
				// erroneously creates new variables internally
				if (p.GetVar().size() != 5)
					throw false;

				mu::varmap_type::const_iterator item = UsedVar.begin();
				for (idx = 0; item != UsedVar.end(); ++item)
				{
					if (&vVarVal[idx++] != item->second)
						throw false;
				}

				// Test lookup of undefined variables
				p.SetExpr(_T("undef1+undef2+undef3"));
				UsedVar = p.GetUsedVar();
				iCount = (int)UsedVar.size();
				if (iCount != 3)
					throw false;

				// the next check will fail if the parser
				// erroneously creates new variables internally
				if (p.GetVar().size() != 5)
					throw false;

				for (item = UsedVar.begin(); item != UsedVar.end(); ++item)
				{
					if (item->second != 0)
						throw false; // all pointers to undefined variables must be null
				}

				// 1 used variables
				p.SetExpr(_T("a+b"));
				UsedVar = p.GetUsedVar();
				iCount = (int)UsedVar.size();
				if (iCount != 2) throw false;
				item = UsedVar.begin();
				for (idx = 0; item != UsedVar.end(); ++item)
					if (&vVarVal[idx++] != item->second) throw false;

			}
			catch (...)
			{
				iStat += 1;
			}

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------
		int ParserTester::TestMultiArg()
		{
			int iStat = 0;
			mu::console() << _T("testing multiarg functions...");

			// from oss-fzz.com: UNKNOWN READ; https://bugs.chromium.org/p/oss-fuzz/issues/detail?id=23330#c1
			iStat += ThrowTest(_T("6, +, +, +, +, +, +, +, +, +, +, +, +, +, +, 1, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +"), ecUNEXPECTED_ARG_SEP, true);

			// misplaced string argument
			iStat += ThrowTest(_T(R"(sin(0?4:("")))"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"(avg(0?4:(""),1))"), ecUNEXPECTED_STR);

			// Compound expressions
			iStat += EqnTest(_T("1,2,3"), 3, true);
			iStat += EqnTest(_T("a,b,c"), 3, true);
			iStat += EqnTest(_T("a=10,b=20,c=a*b"), 200, true);
			iStat += EqnTest(_T("1,\n2,\n3"), 3, true);
			iStat += EqnTest(_T("a,\nb,\nc"), 3, true);
			iStat += EqnTest(_T("a=10,\nb=20,\nc=a*b"), 200, true);
			iStat += EqnTest(_T("1,\r\n2,\r\n3"), 3, true);
			iStat += EqnTest(_T("a,\r\nb,\r\nc"), 3, true);
			iStat += EqnTest(_T("a=10,\r\nb=20,\r\nc=a*b"), 200, true);

			// picking the right argument
			iStat += EqnTest(_T("f1of1(1)"), 1, true);
			iStat += EqnTest(_T("f1of2(1, 2)"), 1, true);
			iStat += EqnTest(_T("f2of2(1, 2)"), 2, true);
			iStat += EqnTest(_T("f1of3(1, 2, 3)"), 1, true);
			iStat += EqnTest(_T("f2of3(1, 2, 3)"), 2, true);
			iStat += EqnTest(_T("f3of3(1, 2, 3)"), 3, true);
			iStat += EqnTest(_T("f1of4(1, 2, 3, 4)"), 1, true);
			iStat += EqnTest(_T("f2of4(1, 2, 3, 4)"), 2, true);
			iStat += EqnTest(_T("f3of4(1, 2, 3, 4)"), 3, true);
			iStat += EqnTest(_T("f4of4(1, 2, 3, 4)"), 4, true);
			iStat += EqnTest(_T("f1of5(1, 2, 3, 4, 5)"), 1, true);
			iStat += EqnTest(_T("f2of5(1, 2, 3, 4, 5)"), 2, true);
			iStat += EqnTest(_T("f3of5(1, 2, 3, 4, 5)"), 3, true);
			iStat += EqnTest(_T("f4of5(1, 2, 3, 4, 5)"), 4, true);
			iStat += EqnTest(_T("f5of5(1, 2, 3, 4, 5)"), 5, true);
			// Too few arguments / Too many arguments
			iStat += EqnTest(_T("1+ping()"), 11, true);
			iStat += EqnTest(_T("ping()+1"), 11, true);
			iStat += EqnTest(_T("2*ping()"), 20, true);
			iStat += EqnTest(_T("ping()*2"), 20, true);
			iStat += EqnTest(_T("ping(1,2)"), 0, false);
			iStat += EqnTest(_T("1+ping(1,2)"), 0, false);
			iStat += EqnTest(_T("f1of1(1,2)"), 0, false);
			iStat += EqnTest(_T("f1of1()"), 0, false);
			iStat += EqnTest(_T("f1of2(1, 2, 3)"), 0, false);
			iStat += EqnTest(_T("f1of2(1)"), 0, false);
			iStat += EqnTest(_T("f1of3(1, 2, 3, 4)"), 0, false);
			iStat += EqnTest(_T("f1of3(1)"), 0, false);
			iStat += EqnTest(_T("f1of4(1, 2, 3, 4, 5)"), 0, false);
			iStat += EqnTest(_T("f1of4(1)"), 0, false);
			iStat += EqnTest(_T("(1,2,3)"), 0, false);
			iStat += EqnTest(_T("1,2,3"), 0, false);
			iStat += EqnTest(_T("(1*a,2,3)"), 0, false);
			iStat += EqnTest(_T("1,2*a,3"), 0, false);

			// correct calculation of arguments
			iStat += EqnTest(_T("min(a, 1)"), 1, true);
			iStat += EqnTest(_T("min(3*2, 1)"), 1, true);
			iStat += EqnTest(_T("min(3*2, 1)"), 6, false);
			iStat += EqnTest(_T("firstArg(2,3,4)"), 2, true);
			iStat += EqnTest(_T("lastArg(2,3,4)"), 4, true);
			iStat += EqnTest(_T("min(3*a+1, 1)"), 1, true);
			iStat += EqnTest(_T("max(3*a+1, 1)"), 4, true);
			iStat += EqnTest(_T("max(3*a+1, 1)*2"), 8, true);
			iStat += EqnTest(_T("2*max(3*a+1, 1)+2"), 10, true);

			// functions with Variable argument count
			iStat += EqnTest(_T("sum(a)"), 1, true);
			iStat += EqnTest(_T("sum(1,2,3)"), 6, true);
			iStat += EqnTest(_T("sum(a,b,c)"), 6, true);
			iStat += EqnTest(_T("sum(1,-max(1,2),3)*2"), 4, true);
			iStat += EqnTest(_T("2*sum(1,2,3)"), 12, true);
			iStat += EqnTest(_T("2*sum(1,2,3)+2"), 14, true);
			iStat += EqnTest(_T("2*sum(-1,2,3)+2"), 10, true);
			iStat += EqnTest(_T("2*sum(-1,2,-(-a))+2"), 6, true);
			iStat += EqnTest(_T("2*sum(-1,10,-a)+2"), 18, true);
			iStat += EqnTest(_T("2*sum(1,2,3)*2"), 24, true);
			iStat += EqnTest(_T("sum(1,-max(1,2),3)*2"), 4, true);
			iStat += EqnTest(_T("sum(1*3, 4, a+2)"), 10, true);
			iStat += EqnTest(_T("sum(1*3, 2*sum(1,2,2), a+2)"), 16, true);
			iStat += EqnTest(_T("sum(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2)"), 24, true);

			// some failures
			iStat += EqnTest(_T("sum()"), 0, false);
			iStat += EqnTest(_T("sum(,)"), 0, false);
			iStat += EqnTest(_T("sum(1,2,)"), 0, false);
			iStat += EqnTest(_T("sum(,1,2)"), 0, false);

			// user data
			iStat += EqnTest(_T("funud0_8()"), 8, true);
			iStat += EqnTest(_T("funud1_16(10)"), 26, true);
			iStat += EqnTest(_T("funud2_24(10, 100)"), 134, true);
			iStat += EqnTest(_T("funud10_32(1,2,3,4,5,6,7,8,9,10)"), 87, true);
			iStat += EqnTest(_T("funud0_9()"), 9, true);
			iStat += EqnTest(_T("funud1_17(10)"), 27, true);
			iStat += EqnTest(_T("funud2_25(10, 100)"), 135, true);
			iStat += EqnTest(_T("funud10_33(1,2,3,4,5,6,7,8,9,10)"), 88, true);
			iStat += EqnTest(_T("sumud_100(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2)"), 124, true);
			iStat += EqnTest(_T("sumud_100()"), 0, false);

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}


		//---------------------------------------------------------------------------
		int ParserTester::TestInfixOprt()
		{
			int iStat(0);
			mu::console() << "testing infix operators...";

			iStat += EqnTest(_T("+1"), +1, true);
			iStat += EqnTest(_T("-(+1)"), -1, true);
			iStat += EqnTest(_T("-(+1)*2"), -2, true);
			iStat += EqnTest(_T("-(+2)*sqrt(4)"), -4, true);
			iStat += EqnTest(_T("3-+a"), 2, true);
			iStat += EqnTest(_T("+1*3"), 3, true);

			iStat += EqnTest(_T("-1"), -1, true);
			iStat += EqnTest(_T("-(-1)"), 1, true);
			iStat += EqnTest(_T("-(-1)*2"), 2, true);
			iStat += EqnTest(_T("-(-2)*sqrt(4)"), 4, true);
			iStat += EqnTest(_T("-_pi"), -MathImpl<double>::CONST_PI, true);
			iStat += EqnTest(_T("-a"), -1, true);
			iStat += EqnTest(_T("-(a)"), -1, true);
			iStat += EqnTest(_T("-(-a)"), 1, true);
			iStat += EqnTest(_T("-(-a)*2"), 2, true);
			iStat += EqnTest(_T("-(8)"), -8, true);
			iStat += EqnTest(_T("-8"), -8, true);
			iStat += EqnTest(_T("-(2+1)"), -3, true);
			iStat += EqnTest(_T("-(f1of1(1+2*3)+1*2)"), -9, true);
			iStat += EqnTest(_T("-(-f1of1(1+2*3)+1*2)"), 5, true);
			iStat += EqnTest(_T("-sin(8)"), -0.989358, true);
			iStat += EqnTest(_T("3-(-a)"), 4, true);
			iStat += EqnTest(_T("3--a"), 4, true);
			iStat += EqnTest(_T("-1*3"), -3, true);

			// Postfix / infix priorities
			iStat += EqnTest(_T("~2#"), 8, true);
			iStat += EqnTest(_T("~f1of1(2)#"), 8, true);
			iStat += EqnTest(_T("~(b)#"), 8, true);
			iStat += EqnTest(_T("(~b)#"), 12, true);
			iStat += EqnTest(_T("~(2#)"), 8, true);
			iStat += EqnTest(_T("~(f1of1(2)#)"), 8, true);
			//
			iStat += EqnTest(_T("-2^2"), -4, true);
			iStat += EqnTest(_T("-(a+b)^2"), -9, true);
			iStat += EqnTest(_T("(-3)^2"), 9, true);
			iStat += EqnTest(_T("-(-2^2)"), 4, true);
			iStat += EqnTest(_T("3+-3^2"), -6, true);
			// The following assumes use of sqr as postfix operator together
			// with a sign operator of low priority:
			iStat += EqnTest(_T("-2'"), -4, true);
			iStat += EqnTest(_T("-(1+1)'"), -4, true);
			iStat += EqnTest(_T("2+-(1+1)'"), -2, true);
			iStat += EqnTest(_T("2+-2'"), -2, true);
			// This is the classic behaviour of the infix sign operator (here: "$") which is
			// now deprecated:
			iStat += EqnTest(_T("$2^2"), 4, true);
			iStat += EqnTest(_T("$(a+b)^2"), 9, true);
			iStat += EqnTest(_T("($3)^2"), 9, true);
			iStat += EqnTest(_T("$($2^2)"), -4, true);
			iStat += EqnTest(_T("3+$3^2"), 12, true);

			// infix operators sharing the first few characters
			iStat += EqnTest(_T("~ 123"), (value_type)123.0 + 2, true);
			iStat += EqnTest(_T("~~ 123"), (value_type)123.0 + 2, true);

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}


		//---------------------------------------------------------------------------
		int ParserTester::TestPostFix()
		{
			int iStat = 0;
			mu::console() << _T("testing postfix operators...");

			// application
			iStat += EqnTest(_T("3{m}+5"), 5.003, true);
			iStat += EqnTest(_T("1000{m}"), 1, true);
			iStat += EqnTest(_T("1000 {m}"), 1, true);
			iStat += EqnTest(_T("(a){m}"), 1e-3, true);
			iStat += EqnTest(_T("a{m}"), 1e-3, true);
			iStat += EqnTest(_T("a {m}"), 1e-3, true);
			iStat += EqnTest(_T("-(a){m}"), -1e-3, true);
			iStat += EqnTest(_T("-2{m}"), -2e-3, true);
			iStat += EqnTest(_T("-2 {m}"), -2e-3, true);
			iStat += EqnTest(_T("f1of1(1000){m}"), 1, true);
			iStat += EqnTest(_T("-f1of1(1000){m}"), -1, true);
			iStat += EqnTest(_T("-f1of1(-1000){m}"), 1, true);
			iStat += EqnTest(_T("f4of4(0,0,0,1000){m}"), 1, true);
			iStat += EqnTest(_T("2+(a*1000){m}"), 3, true);

			// I have added a space between the number and the operator so that
			// systems using libc++ can pass the test (see #123)
			iStat += EqnTest(_T("1 n"), 1e-9, true);

			// can postfix operators "m" und "meg" be told apart properly?
			iStat += EqnTest(_T("2*3000meg+2"), 2 * 3e9 + 2, true);

			// some incorrect results
			iStat += EqnTest(_T("1000{m}"), 0.1, false);
			iStat += EqnTest(_T("(a){m}"), 2, false);
			// failure due to syntax checking
			iStat += ThrowTest(_T("0x"), ecUNASSIGNABLE_TOKEN);  // incomplete hex definition
			iStat += ThrowTest(_T("3+"), ecUNEXPECTED_EOF);
			iStat += ThrowTest(_T("4 + {m}"), ecUNASSIGNABLE_TOKEN);
			iStat += ThrowTest(_T("{m}4"), ecUNASSIGNABLE_TOKEN);
			iStat += ThrowTest(_T("sin({m})"), ecUNASSIGNABLE_TOKEN);
			iStat += ThrowTest(_T("{m} {m}"), ecUNASSIGNABLE_TOKEN);
			iStat += ThrowTest(_T("{m}(8)"), ecUNASSIGNABLE_TOKEN);
			iStat += ThrowTest(_T("4,{m}"), ecUNASSIGNABLE_TOKEN);
			iStat += ThrowTest(_T("-{m}"), ecUNASSIGNABLE_TOKEN);
			iStat += ThrowTest(_T("2(-{m})"), ecUNEXPECTED_PARENS);
			iStat += ThrowTest(_T("2({m})"), ecUNEXPECTED_PARENS);

			iStat += ThrowTest(_T("multi*1.0"), ecUNASSIGNABLE_TOKEN);

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------
		int ParserTester::TestExpression()
		{
			int iStat = 0;
			mu::console() << _T("testing expression samples...");

			value_type b = 2;

			iStat += EqnTest(_T("f0()"), 42, true);
			iStat += EqnTest(_T("b^2"), 4, true);
			iStat += EqnTest(_T("b^1"), 2, true);
			iStat += EqnTest(_T("b^0"), 1, true);
			iStat += EqnTest(_T("b^-1"), 0.5, true);

			// Optimization
			iStat += EqnTest(_T("2*b*5"), 20, true);
			iStat += EqnTest(_T("2*b*5 + 4*b"), 28, true);
			iStat += EqnTest(_T("2*a/3"), 2.0 / 3.0, true);

			// Addition auf cmVARMUL
			iStat += EqnTest(_T("3+b"), b + 3, true);
			iStat += EqnTest(_T("b+3"), b + 3, true);
			iStat += EqnTest(_T("b*3+2"), b * 3 + 2, true);
			iStat += EqnTest(_T("3*b+2"), b * 3 + 2, true);
			iStat += EqnTest(_T("2+b*3"), b * 3 + 2, true);
			iStat += EqnTest(_T("2+3*b"), b * 3 + 2, true);
			iStat += EqnTest(_T("b+3*b"), b + 3 * b, true);
			iStat += EqnTest(_T("3*b+b"), b + 3 * b, true);

			iStat += EqnTest(_T("2+b*3+b"), 2 + b * 3 + b, true);
			iStat += EqnTest(_T("b+2+b*3"), b + 2 + b * 3, true);

			iStat += EqnTest(_T("(2*b+1)*4"), (2 * b + 1) * 4, true);
			iStat += EqnTest(_T("4*(2*b+1)"), (2 * b + 1) * 4, true);

			// operator precedences
			iStat += EqnTest(_T("1+2-3*4/5^6"), 2.99923, true);
			iStat += EqnTest(_T("1^2/3*4-5+6"), 2.33333333, true);
			iStat += EqnTest(_T("1+2*3"), 7, true);
			iStat += EqnTest(_T("1+2*3"), 7, true);
			iStat += EqnTest(_T("(1+2)*3"), 9, true);
			iStat += EqnTest(_T("(1+2)*(-3)"), -9, true);
			iStat += EqnTest(_T("2/4"), 0.5, true);

			iStat += EqnTest(_T("exp(ln(7))"), 7, true);
			iStat += EqnTest(_T("e^ln(7)"), 7, true);
			iStat += EqnTest(_T("e^(ln(7))"), 7, true);
			iStat += EqnTest(_T("(e^(ln(7)))"), 7, true);
			iStat += EqnTest(_T("1-(e^(ln(7)))"), -6, true);
			iStat += EqnTest(_T("2*(e^(ln(7)))"), 14, true);
			iStat += EqnTest(_T("10^log(5)"), pow(10.0, log(5.0)), true);
			iStat += EqnTest(_T("10^log10(5)"), 5, true);
			iStat += EqnTest(_T("2^log2(4)"), 4, true);
			iStat += EqnTest(_T("-(sin(0)+1)"), -1, true);
			iStat += EqnTest(_T("-(2^1.1)"), -2.14354692, true);

			iStat += EqnTest(_T("(cos(2.41)/b)"), -0.372056, true);
			iStat += EqnTest(_T("(1*(2*(3*(4*(5*(6*(a+b)))))))"), 2160, true);
			iStat += EqnTest(_T("(1*(2*(3*(4*(5*(6*(7*(a+b))))))))"), 15120, true);
			iStat += EqnTest(_T("(a/((((b+(((e*(((((pi*((((3.45*((pi+a)+pi))+b)+b)*a))+0.68)+e)+a)/a))+a)+b))+b)*a)-pi))"), 0.00377999, true);

			// long formula (Reference: Matlab)
			iStat += EqnTest(
				_T("(((-9))-e/(((((((pi-(((-7)+(-3)/4/e))))/(((-5))-2)-((pi+(-0))*(sqrt((e+e))*(-8))*(((-pi)+(-pi)-(-9)*(6*5))")
				_T("/(-e)-e))/2)/((((sqrt(2/(-e)+6)-(4-2))+((5/(-2))/(1*(-pi)+3))/8)*pi*((pi/((-2)/(-6)*1*(-1))*(-6)+(-e)))))/")
				_T("((e+(-2)+(-e)*((((-3)*9+(-e)))+(-9)))))))-((((e-7+(((5/pi-(3/1+pi)))))/e)/(-5))/(sqrt((((((1+(-7))))+((((-")
				_T("e)*(-e)))-8))*(-5)/((-e)))*(-6)-((((((-2)-(-9)-(-e)-1)/3))))/(sqrt((8+(e-((-6))+(9*(-9))))*(((3+2-8))*(7+6")
				_T("+(-5))+((0/(-e)*(-pi))+7)))+(((((-e)/e/e)+((-6)*5)*e+(3+(-5)/pi))))+pi))/sqrt((((9))+((((pi))-8+2))+pi))/e")
				_T("*4)*((-5)/(((-pi))*(sqrt(e)))))-(((((((-e)*(e)-pi))/4+(pi)*(-9)))))))+(-pi)"), -12.23016549, true);

			// long formula (Reference: Matlab)
			iStat += EqnTest(
				_T("(atan(sin((((((((((((((((pi/cos((a/((((0.53-b)-pi)*e)/b))))+2.51)+a)-0.54)/0.98)+b)*b)+e)/a)+b)+a)+b)+pi)/e")
				_T(")+a)))*2.77)"), -2.16995656, true);

			// long formula (Reference: Matlab)
			iStat += EqnTest(_T("1+2-3*4/5^6*(2*(1-5+(3*7^9)*(4+6*7-3)))+12"), -7995810.09926, true);

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}



		//---------------------------------------------------------------------------
		int ParserTester::TestIfThenElse()
		{
			int iStat = 0;
			mu::console() << _T("testing if-then-else operator...");

			// from oss-fuzz.com: https://bugs.chromium.org/p/oss-fuzz/issues/detail?id=24167
			iStat += ThrowTest(_T(R"(0^3^avg(0>3?4:(""),0^3?4:("")))"), ecUNEXPECTED_STR);
			// derivatives
			iStat += ThrowTest(_T(R"(avg(0?(""):4,1))"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"(avg(0>3?4:(""),0^3?4:("")))"), ecUNEXPECTED_STR);

			iStat += ThrowTest(_T(R"(0?4:(""))"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"((0)?4:(""))"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"((0>3)?4:(""))"), ecUNEXPECTED_STR);
			iStat += ThrowTest(_T(R"(0>3?4:(""))"), ecUNEXPECTED_STR);

			// from oss-fuzz.com: https://oss-fuzz.com/testcase-detail/4777121158529024
			iStat += ThrowTest(_T("3!=min(0?2>2,2>5,1:6)"), ecUNEXPECTED_ARG_SEP);

			// Test error detection
			iStat += ThrowTest(_T(":3"), ecUNEXPECTED_CONDITIONAL);
			iStat += ThrowTest(_T("? 1 : 2"), ecUNEXPECTED_CONDITIONAL);
			iStat += ThrowTest(_T("(a<b) ? (b<c) ? 1 : 2"), ecMISSING_ELSE_CLAUSE);
			iStat += ThrowTest(_T("(a<b) ? 1"), ecMISSING_ELSE_CLAUSE);
			iStat += ThrowTest(_T("(a<b) ? a"), ecMISSING_ELSE_CLAUSE);
			iStat += ThrowTest(_T("(a<b) ? a+b"), ecMISSING_ELSE_CLAUSE);
			iStat += ThrowTest(_T("a : b"), ecMISPLACED_COLON);
			iStat += ThrowTest(_T("1 : 2"), ecMISPLACED_COLON);
			iStat += ThrowTest(_T("(1) ? 1 : 2 : 3"), ecMISPLACED_COLON);
			iStat += ThrowTest(_T("(true) ? 1 : 2 : 3"), ecUNASSIGNABLE_TOKEN);

			// from oss-fzz.com: UNKNOWN READ; https://bugs.chromium.org/p/oss-fuzz/issues/detail?id=22922#c1
			iStat += ThrowTest(_T("1?2:0?(7:1)"), ecMISPLACED_COLON);

			// from oss-fuzz.com: https://bugs.chromium.org/p/oss-fuzz/issues/detail?id=22938
			iStat += ThrowTest(_T("sum(0?1,0,0:3)"), ecUNEXPECTED_ARG_SEP);
			iStat += ThrowTest(_T("sum(0?(1,0,0):3)"), ecUNEXPECTED_ARG);
			iStat += ThrowTest(_T("sum(2>3?2,4,2:4)"), ecUNEXPECTED_ARG_SEP);
			iStat += ThrowTest(_T("sum(2>3?2,4,sin(2):4)"), ecUNEXPECTED_ARG_SEP);
			iStat += ThrowTest(_T("sum(2>3?sin(2),4,2:4)"), ecUNEXPECTED_ARG_SEP);
			iStat += ThrowTest(_T("sum(2>3?sin(a),4,2:4)"), ecUNEXPECTED_ARG_SEP);
			iStat += ThrowTest(_T("sum(2>3?sin(2),4,2:4)"), ecUNEXPECTED_ARG_SEP);

			iStat += EqnTest(_T("1 ? 128 : 255"), 128, true);
			iStat += EqnTest(_T("1<2 ? 128 : 255"), 128, true);
			iStat += EqnTest(_T("a<b ? 128 : 255"), 128, true);
			iStat += EqnTest(_T("(a<b) ? 128 : 255"), 128, true);
			iStat += EqnTest(_T("(1) ? 10 : 11"), 10, true);
			iStat += EqnTest(_T("(0) ? 10 : 11"), 11, true);
			iStat += EqnTest(_T("(1) ? a+b : c+d"), 3, true);
			iStat += EqnTest(_T("(0) ? a+b : c+d"), 1, true);
			iStat += EqnTest(_T("(1) ? 0 : 1"), 0, true);
			iStat += EqnTest(_T("(0) ? 0 : 1"), 1, true);
			iStat += EqnTest(_T("(a<b) ? 10 : 11"), 10, true);
			iStat += EqnTest(_T("(a>b) ? 10 : 11"), 11, true);
			iStat += EqnTest(_T("(a<b) ? c : d"), 3, true);
			iStat += EqnTest(_T("(a>b) ? c : d"), -2, true);

			iStat += EqnTest(_T("(a>b) ? 1 : 0"), 0, true);
			iStat += EqnTest(_T("((a>b) ? 1 : 0) ? 1 : 2"), 2, true);
			iStat += EqnTest(_T("((a>b) ? 1 : 0) ? 1 : sum((a>b) ? 1 : 2)"), 2, true);
			iStat += EqnTest(_T("((a>b) ? 0 : 1) ? 1 : sum((a>b) ? 1 : 2)"), 1, true);

			iStat += EqnTest(_T("sum((a>b) ? 1 : 2)"), 2, true);
			iStat += EqnTest(_T("sum((1) ? 1 : 2)"), 1, true);
			iStat += EqnTest(_T("sum((a>b) ? 1 : 2, 100)"), 102, true);
			iStat += EqnTest(_T("sum((1) ? 1 : 2, 100)"), 101, true);
			iStat += EqnTest(_T("sum(3, (a>b) ? 3 : 10)"), 13, true);
			iStat += EqnTest(_T("sum(3, (a<b) ? 3 : 10)"), 6, true);
			iStat += EqnTest(_T("10*sum(3, (a>b) ? 3 : 10)"), 130, true);
			iStat += EqnTest(_T("10*sum(3, (a<b) ? 3 : 10)"), 60, true);
			iStat += EqnTest(_T("sum(3, (a>b) ? 3 : 10)*10"), 130, true);
			iStat += EqnTest(_T("sum(3, (a<b) ? 3 : 10)*10"), 60, true);
			iStat += EqnTest(_T("(a<b) ? sum(3, (a<b) ? 3 : 10)*10 : 99"), 60, true);
			iStat += EqnTest(_T("(a>b) ? sum(3, (a<b) ? 3 : 10)*10 : 99"), 99, true);
			iStat += EqnTest(_T("(a<b) ? sum(3, (a<b) ? 3 : 10,10,20)*10 : 99"), 360, true);
			iStat += EqnTest(_T("(a>b) ? sum(3, (a<b) ? 3 : 10,10,20)*10 : 99"), 99, true);
			iStat += EqnTest(_T("(a>b) ? sum(3, (a<b) ? 3 : 10,10,20)*10 : sum(3, (a<b) ? 3 : 10)*10"), 60, true);

			// todo: also add for muParserX!
			iStat += EqnTest(_T("(a<b)&&(a<b) ? 128 : 255"), 128, true);
			iStat += EqnTest(_T("(a>b)&&(a<b) ? 128 : 255"), 255, true);
			iStat += EqnTest(_T("(1<2)&&(1<2) ? 128 : 255"), 128, true);
			iStat += EqnTest(_T("(1>2)&&(1<2) ? 128 : 255"), 255, true);
			iStat += EqnTest(_T("((1<2)&&(1<2)) ? 128 : 255"), 128, true);
			iStat += EqnTest(_T("((1>2)&&(1<2)) ? 128 : 255"), 255, true);
			iStat += EqnTest(_T("((a<b)&&(a<b)) ? 128 : 255"), 128, true);
			iStat += EqnTest(_T("((a>b)&&(a<b)) ? 128 : 255"), 255, true);

			iStat += EqnTest(_T("1>0 ? 1>2 ? 128 : 255 : 1>0 ? 32 : 64"), 255, true);
			iStat += EqnTest(_T("1>0 ? 1>2 ? 128 : 255 :(1>0 ? 32 : 64)"), 255, true);
			iStat += EqnTest(_T("1>0 ? 1>0 ? 128 : 255 : 1>2 ? 32 : 64"), 128, true);
			iStat += EqnTest(_T("1>0 ? 1>0 ? 128 : 255 :(1>2 ? 32 : 64)"), 128, true);
			iStat += EqnTest(_T("1>2 ? 1>2 ? 128 : 255 : 1>0 ? 32 : 64"), 32, true);
			iStat += EqnTest(_T("1>2 ? 1>0 ? 128 : 255 : 1>2 ? 32 : 64"), 64, true);
			iStat += EqnTest(_T("1>0 ? 50 :  1>0 ? 128 : 255"), 50, true);
			iStat += EqnTest(_T("1>0 ? 50 : (1>0 ? 128 : 255)"), 50, true);
			iStat += EqnTest(_T("1>0 ? 1>0 ? 128 : 255 : 50"), 128, true);
			iStat += EqnTest(_T("1>2 ? 1>2 ? 128 : 255 : 1>0 ? 32 : 1>2 ? 64 : 16"), 32, true);
			iStat += EqnTest(_T("1>2 ? 1>2 ? 128 : 255 : 1>0 ? 32 :(1>2 ? 64 : 16)"), 32, true);
			iStat += EqnTest(_T("1>0 ? 1>2 ? 128 : 255 :  1>0 ? 32 :1>2 ? 64 : 16"), 255, true);
			iStat += EqnTest(_T("1>0 ? 1>2 ? 128 : 255 : (1>0 ? 32 :1>2 ? 64 : 16)"), 255, true);
			iStat += EqnTest(_T("1 ? 0 ? 128 : 255 : 1 ? 32 : 64"), 255, true);

			// assignment operators
			iStat += EqnTest(_T("a= 0 ? 128 : 255, a"), 255, true);
			iStat += EqnTest(_T("a=((a>b)&&(a<b)) ? 128 : 255, a"), 255, true);
			iStat += EqnTest(_T("c=(a<b)&&(a<b) ? 128 : 255, c"), 128, true);
			iStat += EqnTest(_T("0 ? a=a+1 : 666, a"), 1, true);
			iStat += EqnTest(_T("1?a=10:a=20, a"), 10, true);
			iStat += EqnTest(_T("0?a=10:a=20, a"), 20, true);
			iStat += EqnTest(_T("0?a=sum(3,4):10, a"), 1, true);  // a should not change its value due to lazy calculation

			iStat += EqnTest(_T("a=1?b=1?3:4:5, a"), 3, true);
			iStat += EqnTest(_T("a=1?b=1?3:4:5, b"), 3, true);
			iStat += EqnTest(_T("a=0?b=1?3:4:5, a"), 5, true);
			iStat += EqnTest(_T("a=0?b=1?3:4:5, b"), 2, true);

			iStat += EqnTest(_T("a=1?5:b=1?3:4, a"), 5, true);
			iStat += EqnTest(_T("a=1?5:b=1?3:4, b"), 2, true);
			iStat += EqnTest(_T("a=0?5:b=1?3:4, a"), 3, true);
			iStat += EqnTest(_T("a=0?5:b=1?3:4, b"), 3, true);

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		//---------------------------------------------------------------------------
		int ParserTester::TestException()
		{
			int  iStat = 0;
			mu::console() << _T("testing error codes...");

			iStat += ThrowTest(_T("3+"), ecUNEXPECTED_EOF);
			iStat += ThrowTest(_T("3+)"), ecUNEXPECTED_PARENS);
			iStat += ThrowTest(_T("()"), ecUNEXPECTED_PARENS);
			iStat += ThrowTest(_T("3+()"), ecUNEXPECTED_PARENS);
			iStat += ThrowTest(_T("sin(3,4)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("sin()"), ecTOO_FEW_PARAMS);
			iStat += ThrowTest(_T("(1+2"), ecMISSING_PARENS);
			iStat += ThrowTest(_T("sin(3)3"), ecUNEXPECTED_VAL);
			iStat += ThrowTest(_T("sin(3)xyz"), ecUNASSIGNABLE_TOKEN);
			iStat += ThrowTest(_T("sin(3)cos(3)"), ecUNEXPECTED_FUN);
			iStat += ThrowTest(_T("a+b+c=10"), ecUNEXPECTED_OPERATOR);
			iStat += ThrowTest(_T("a=b=3"), ecUNEXPECTED_OPERATOR);

			// functions without parameter
			iStat += ThrowTest(_T("3+ping(2)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("3+ping(a+2)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("3+ping(sin(a)+2)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("3+ping(1+sin(a))"), ecTOO_MANY_PARAMS);

			// String function related
			iStat += ThrowTest(_T("valueof(\"xxx\")"), 999, false);
			iStat += ThrowTest(_T("valueof()"), ecUNEXPECTED_PARENS);
			iStat += ThrowTest(_T("1+valueof(\"abc\""), ecMISSING_PARENS);
			iStat += ThrowTest(_T("valueof(\"abc\""), ecMISSING_PARENS);
			iStat += ThrowTest(_T("valueof(\"abc"), ecUNTERMINATED_STRING);
			iStat += ThrowTest(_T("valueof(\"abc\",3)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("valueof(3)"), ecSTRING_EXPECTED);
			iStat += ThrowTest(_T("sin(\"abc\")"), ecVAL_EXPECTED);
			iStat += ThrowTest(_T("valueof(\"\\\"abc\\\"\")"), 999, false);
			iStat += ThrowTest(_T("\"hello world\""), ecSTR_RESULT);
			iStat += ThrowTest(_T("(\"hello world\")"), ecSTR_RESULT);
			iStat += ThrowTest(_T("\"abcd\"+100"), ecSTR_RESULT);
			iStat += ThrowTest(_T("\"a\"+\"b\""), ecSTR_RESULT);
			iStat += ThrowTest(_T("strfun1(\"100\",3)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("strfun2(\"100\",3,5)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("strfun3(\"100\",3,5,6)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("strfun2(\"100\")"), ecTOO_FEW_PARAMS);
			iStat += ThrowTest(_T("strfun3(\"100\",6)"), ecTOO_FEW_PARAMS);
			iStat += ThrowTest(_T("strfun2(1,1)"), ecSTRING_EXPECTED);
			iStat += ThrowTest(_T("strfun2(a,1)"), ecSTRING_EXPECTED);
			iStat += ThrowTest(_T("strfun2(1,1,1)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("strfun2(a,1,1)"), ecTOO_MANY_PARAMS);
			iStat += ThrowTest(_T("strfun3(1,2,3)"), ecSTRING_EXPECTED);
			iStat += ThrowTest(_T("strfun3(1, \"100\",3)"), ecSTRING_EXPECTED);
			iStat += ThrowTest(_T("strfun3(\"1\", \"100\",3)"), ecVAL_EXPECTED);
			iStat += ThrowTest(_T("strfun3(\"1\", 3, \"100\")"), ecVAL_EXPECTED);
			iStat += ThrowTest(_T("strfun3(\"1\", \"100\", \"100\", \"100\")"), ecTOO_MANY_PARAMS);

			// assignment operator
			iStat += ThrowTest(_T("3=4"), ecUNEXPECTED_OPERATOR);
			iStat += ThrowTest(_T("sin(8)=4"), ecUNEXPECTED_OPERATOR);
			iStat += ThrowTest(_T("\"test\"=a"), ecSTR_RESULT);

			// <ibg 20090529>
			// this is now legal, for reference see:
			// https://sourceforge.net/forum/message.php?msg_id=7411373
			//      iStat += ThrowTest( _T("sin=9"), ecUNEXPECTED_OPERATOR);
			// </ibg>

			iStat += ThrowTest(_T("(8)=5"), ecUNEXPECTED_OPERATOR);
			iStat += ThrowTest(_T("(a)=5"), ecUNEXPECTED_OPERATOR);
			iStat += ThrowTest(_T("a=\"tttt\""), ecOPRT_TYPE_CONFLICT);

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			return iStat;
		}

		int ParserTester::TestLocalization()
		{
			int  iStat = 0;
			mu::console() << _T("testing localization...");

			iStat += EqnTestLocalized(_T("1,2"), 1.2, true);

			if (iStat == 0)
				mu::console() << _T("passed") << endl;
			else
				mu::console() << _T("\n  failed with ") << iStat << _T(" errors") << endl;

			// Reset the locale to the "C" locale.
			Parser().ResetLocale();

			return iStat;
		}


		void ParserTester::AddTest(testfun_type a_pFun)
		{
			m_vTestFun.push_back(a_pFun);
		}

		int ParserTester::Run()
		{
			int iStat = 0;
			try
			{
				for (int i = 0; i < (int)m_vTestFun.size(); ++i)
					iStat += (this->*m_vTestFun[i])();
			}
			catch (Parser::exception_type& e)
			{
				mu::console() << "\n" << e.GetMsg() << endl;
				mu::console() << e.GetToken() << endl;
				Abort();
			}
			catch (std::exception& e)
			{
				mu::console() << e.what() << endl;
				Abort();
			}
			catch (...)
			{
				mu::console() << "Internal error";
				Abort();
			}

			if (iStat == 0)
			{
				mu::console() << "Test passed (" << ParserTester::c_iCount << " expressions)" << endl;
			}
			else
			{
				mu::console() << "Test failed with " << iStat
					<< " errors (" << ParserTester::c_iCount
					<< " expressions)" << endl;
			}
			ParserTester::c_iCount = 0;
			return iStat;
		}


		//---------------------------------------------------------------------------
		int ParserTester::ThrowTest(const string_type& a_str, int a_iErrc, bool a_expectedToFail)
		{
			ParserTester::c_iCount++;

			try
			{
				value_type fVal[] = { 1,1,1 };
				Parser p;

				p.DefineVar(_T("a"), &fVal[0]);
				p.DefineVar(_T("b"), &fVal[1]);
				p.DefineVar(_T("c"), &fVal[2]);
				p.DefinePostfixOprt(_T("{m}"), Milli);
				p.DefinePostfixOprt(_T("m"), Milli);
				p.DefineFun(_T("ping"), Ping);
				p.DefineFun(_T("valueof"), ValueOf);
				p.DefineFun(_T("strfun1"), StrFun1);
				p.DefineFun(_T("strfun2"), StrFun2);
				p.DefineFun(_T("strfun3"), StrFun3);
				p.DefineFun(_T("strfun4"), StrFun4);
				p.DefineFun(_T("strfun5"), StrFun5);
				p.DefineFun(_T("strfun6"), StrFun6);
				p.SetExpr(a_str);
				//				p.EnableDebugDump(1, 0);
				p.Eval();
			}
			catch (ParserError& e)
			{
				// output the formula in case of an failed test
				if (a_expectedToFail == false || (a_expectedToFail == true && a_iErrc != e.GetCode()))
				{
					mu::console() << _T("\n  ")
						<< _T("Expression: ") << a_str
						<< _T("  Code:") << e.GetCode() << _T("(") << e.GetMsg() << _T(")")
						<< _T("  Expected:") << a_iErrc;
				}

				return (a_iErrc == e.GetCode()) ? 0 : 1;
			}

			// if a_expectedToFail == false no exception is expected
			bool bRet((a_expectedToFail == false) ? 0 : 1);
			if (bRet == 1)
			{
				mu::console() << _T("\n  ")
					<< _T("Expression: ") << a_str
					<< _T("  did evaluate; Expected error:") << a_iErrc;
			}

			return bRet;
		}

		//---------------------------------------------------------------------------
		/** \brief Evaluate a test expression.

			\return 1 in case of a failure, 0 otherwise.
		*/
		int ParserTester::EqnTestWithVarChange(const string_type& a_str,
			double a_fVar1,
			double a_fRes1,
			double a_fVar2,
			double a_fRes2)
		{
			ParserTester::c_iCount++;

			try
			{
				value_type fVal[2] = { -999, -999 }; // should be equal

				Parser  p;
				value_type var = 0;

				// variable
				p.DefineVar(_T("a"), &var);
				p.SetExpr(a_str);

				var = a_fVar1;
				fVal[0] = p.Eval();

				var = a_fVar2;
				fVal[1] = p.Eval();

				if (fabs(a_fRes1 - fVal[0]) > 0.0000000001)
					throw std::runtime_error("incorrect result (first pass)");

				if (fabs(a_fRes2 - fVal[1]) > 0.0000000001)
					throw std::runtime_error("incorrect result (second pass)");
			}
			catch (Parser::exception_type& e)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.GetMsg() << _T(")");
				return 1;
			}
			catch (std::exception& e)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.what() << _T(")");
				return 1;  // always return a failure since this exception is not expected
			}
			catch (...)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (unexpected exception)");
				return 1;  // exceptions other than ParserException are not allowed
			}

			return 0;
		}

		//---------------------------------------------------------------------------
		/** \brief Evaluate a test expression.

			\return 1 in case of a failure, 0 otherwise.
		*/
		int ParserTester::EqnTestLocalized(const string_type& a_str, double a_fRes, bool a_fPass)
		{
			ParserTester::c_iCount++;

			try
			{
				Parser  p;
				value_type var[2] = { 1, 2 };

				// variable
				p.SetDecSep(',');
				p.SetArgSep(';');
				p.SetThousandsSep('.');

				p.DefineVar(_T("a"), &var[0]);
				p.DefineVar(_T("b"), &var[1]);
				p.SetExpr(a_str);

				auto result = p.Eval();

				if (fabs(result - a_fRes) > 0.0000000001)
					throw std::runtime_error("incorrect result (first pass)");
			}
			catch (Parser::exception_type& e)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.GetMsg() << _T(")");
				return 1;
			}
			catch (std::exception& e)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.what() << _T(")");
				return 1;  // always return a failure since this exception is not expected
			}
			catch (...)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (unexpected exception)");
				return 1;  // exceptions other than ParserException are not allowed
			}

			return 0;
		}

		//---------------------------------------------------------------------------
		/** \brief Evaluate a tet expression.

			\return 1 in case of a failure, 0 otherwise.
		*/
		int ParserTester::EqnTest(const string_type& a_str, double a_fRes, bool a_fPass)
		{
			ParserTester::c_iCount++;
			int iRet(0);
			value_type fVal[6] = { -999, -998, -997, -996, -995, -994 }; // initially should be different

			try
			{
				std::unique_ptr<Parser> p1;
				Parser  p2, p3;   // three parser objects
								  // they will be used for testing copy and assignment operators
				// p1 is a pointer since i'm going to delete it in order to test if
				// parsers after copy construction still refer to members of it.
				// !! If this is the case this function will crash !!

				p1.reset(new mu::Parser());
				// Add constants
				p1->DefineConst(_T("pi"), MathImpl<value_type>::CONST_PI);
				p1->DefineConst(_T("e"), MathImpl<value_type>::CONST_E);
				p1->DefineConst(_T("const"), 1);
				p1->DefineConst(_T("const1"), 2);
				p1->DefineConst(_T("const2"), 3);
				// string constants
				p1->DefineStrConst(_T("str1"), _T("1.11"));
				p1->DefineStrConst(_T("str2"), _T("2.22"));
				// variables
				value_type vVarVal[] = { 1, 2, 3, -2 };
				p1->DefineVar(_T("a"), &vVarVal[0]);
				p1->DefineVar(_T("aa"), &vVarVal[1]);
				p1->DefineVar(_T("b"), &vVarVal[1]);
				p1->DefineVar(_T("c"), &vVarVal[2]);
				p1->DefineVar(_T("d"), &vVarVal[3]);

				// custom value ident functions
				p1->AddValIdent(&ParserTester::IsHexVal);

				// functions
				p1->DefineFun(_T("ping"), Ping);
				p1->DefineFun(_T("f0"), f0);        // no parameter
				p1->DefineFun(_T("f1of1"), f1of1);  // one parameter
				p1->DefineFun(_T("f1of2"), f1of2);  // two parameter
				p1->DefineFun(_T("f2of2"), f2of2);
				p1->DefineFun(_T("f1of3"), f1of3);  // three parameter
				p1->DefineFun(_T("f2of3"), f2of3);
				p1->DefineFun(_T("f3of3"), f3of3);
				p1->DefineFun(_T("f1of4"), f1of4);  // four parameter
				p1->DefineFun(_T("f2of4"), f2of4);
				p1->DefineFun(_T("f3of4"), f3of4);
				p1->DefineFun(_T("f4of4"), f4of4);
				p1->DefineFun(_T("f1of5"), f1of5);  // five parameter
				p1->DefineFun(_T("f2of5"), f2of5);
				p1->DefineFun(_T("f3of5"), f3of5);
				p1->DefineFun(_T("f4of5"), f4of5);
				p1->DefineFun(_T("f5of5"), f5of5);

				// binary operators
				p1->DefineOprt(_T("add"), add, 0);
				p1->DefineOprt(_T("++"), add, 0);
				p1->DefineOprt(_T("&"), land, prLAND);

				// sample functions
				p1->DefineFun(_T("min"), Min);
				p1->DefineFun(_T("max"), Max);
				p1->DefineFun(_T("sum"), Sum);
				p1->DefineFun(_T("valueof"), ValueOf);
				p1->DefineFun(_T("atof"), StrToFloat);
				p1->DefineFun(_T("strfun1"), StrFun1);
				p1->DefineFun(_T("strfun2"), StrFun2);
				p1->DefineFun(_T("strfun3"), StrFun3);
				p1->DefineFun(_T("strfun4"), StrFun4);
				p1->DefineFun(_T("strfun5"), StrFun5);
				p1->DefineFun(_T("strfun6"), StrFun6);
				p1->DefineFun(_T("lastArg"), LastArg);
				p1->DefineFun(_T("firstArg"), FirstArg);
				p1->DefineFun(_T("order"), FirstArg);

				// functions with user data
				p1->DefineFunUserData(_T("funud0_8"), FunUd0, reinterpret_cast<void*>(8));
				p1->DefineFunUserData(_T("funud1_16"), FunUd1, reinterpret_cast<void*>(16));
				p1->DefineFunUserData(_T("funud2_24"), FunUd2, reinterpret_cast<void*>(24));
				p1->DefineFunUserData(_T("funud10_32"), FunUd10, reinterpret_cast<void*>(32));
				p1->DefineFunUserData(_T("funud0_9"), FunUd0, reinterpret_cast<void*>(9));
				p1->DefineFunUserData(_T("funud1_17"), FunUd1, reinterpret_cast<void*>(17));
				p1->DefineFunUserData(_T("funud2_25"), FunUd2, reinterpret_cast<void*>(25));
				p1->DefineFunUserData(_T("funud10_33"), FunUd10, reinterpret_cast<void*>(33));
				p1->DefineFunUserData(_T("strfunud3_10"), StrFunUd3, reinterpret_cast<void*>(10));
				p1->DefineFunUserData(_T("sumud_100"), SumUd, reinterpret_cast<void*>(100));

				// infix / postfix operator
				// Note: Identifiers used here do not have any meaning
				//       they are mere placeholders to test certain features.
				p1->DefineInfixOprt(_T("$"), sign, prPOW + 1);  // sign with high priority
				p1->DefineInfixOprt(_T("~"), plus2);          // high priority
				p1->DefineInfixOprt(_T("~~"), plus2);
				p1->DefinePostfixOprt(_T("{m}"), Milli);
				p1->DefinePostfixOprt(_T("{M}"), Mega);
				p1->DefinePostfixOprt(_T("m"), Milli);
				p1->DefinePostfixOprt(_T("n"), [](float value){return value * 1E-9f;});
				p1->DefinePostfixOprt(_T("meg"), Mega);
				p1->DefinePostfixOprt(_T("#"), times3);
				p1->DefinePostfixOprt(_T("'"), sqr);
				p1->SetExpr(a_str);

				// Test bytecode integrity
				// String parsing and bytecode parsing must yield the same result
				fVal[0] = p1->Eval(); // result from stringparsing
				fVal[1] = p1->Eval(); // result from bytecode
				if (fVal[0] != fVal[1])
					throw Parser::exception_type(_T("Bytecode / string parsing mismatch."));

				// Test copy and assignment operators
				try
				{
					// Test copy constructor
					std::vector<mu::Parser> vParser;
					vParser.push_back(*(p1.get()));
					mu::Parser p4 = vParser[0];   // take parser from vector

					// destroy the originals from p2
					vParser.clear();              // delete the vector
					p1.reset(nullptr);

					fVal[2] = p4.Eval();

					// Test assignment operator
					// additionally  disable Optimizer this time
					mu::Parser p5;
					p5 = p4;
					p5.EnableOptimizer(false);
					fVal[3] = p5.Eval();

					// Test Eval function for multiple return values
					// use p2 since it has the optimizer enabled!
					int nNum;
					p4.SetExpr(a_str); // reset bytecode to trigger #94 (https://github.com/beltoforion/muparser/issues/94)
					value_type* v = p4.Eval(nNum);
					fVal[4] = v[nNum - 1];

					v = p4.Eval(nNum);
					fVal[5] = v[nNum - 1];

				}
				catch (std::exception& e)
				{
					mu::console() << _T("\n  ") << e.what() << _T("\n");
				}

				// limited floating point accuracy requires the following test
				bool bCloseEnough(true);
				for (unsigned i = 0; i < sizeof(fVal) / sizeof(value_type); ++i)
				{
					bCloseEnough &= (fabs(a_fRes - fVal[i]) <= fabs(fVal[i] * 0.00001));

					// The tests equations never result in infinity, if they do thats a bug.
					// reference:
					// http://sourceforge.net/projects/muparser/forums/forum/462843/topic/5037825
					bCloseEnough &= std::isfinite(fVal[i]);
				}

				iRet = ((bCloseEnough && a_fPass) || (!bCloseEnough && !a_fPass)) ? 0 : 1;


				if (iRet == 1)
				{
					mu::console() << _T("\n  fail: ") << a_str.c_str()
						<< _T(" (incorrect result; expected: ") << a_fRes
						<< _T(" ;calculated: ") << fVal[0] << _T(",")
						<< fVal[1] << _T(",")
						<< fVal[2] << _T(",")
						<< fVal[3] << _T(",")
						<< fVal[4] << _T(",")
						<< fVal[5] << _T(").");
				}
			}
			catch (Parser::exception_type& e)
			{
				if (a_fPass)
				{
					if (fVal[0] != fVal[2] && fVal[0] != -999 && fVal[1] != -998)
						mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (copy construction)");
					else
						mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.GetMsg() << _T(")");
					return 1;
				}
			}
			catch (std::exception& e)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (") << e.what() << _T(")");
				return 1;  // always return a failure since this exception is not expected
			}
			catch (...)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (unexpected exception)");
				return 1;  // exceptions other than ParserException are not allowed
			}

			return iRet;
		}

		//---------------------------------------------------------------------------
		int ParserTester::EqnTestInt(const string_type& a_str, double a_fRes, bool a_fPass)
		{
			ParserTester::c_iCount++;

			value_type vVarVal[] = { 1, 2, 3 };   // variable values
			int iRet(0);

			try
			{
				value_type fVal[2] = { -99, -999 };   // results: initially should be different
				ParserInt p;
				p.DefineConst(_T("const1"), 1);
				p.DefineConst(_T("const2"), 2);
				p.DefineVar(_T("a"), &vVarVal[0]);
				p.DefineVar(_T("b"), &vVarVal[1]);
				p.DefineVar(_T("c"), &vVarVal[2]);

				p.SetExpr(a_str);
				fVal[0] = p.Eval(); // result from stringparsing
				fVal[1] = p.Eval(); // result from bytecode

				if (fVal[0] != fVal[1])
					throw Parser::exception_type(_T("Bytecode corrupt."));

				iRet = ((a_fRes == fVal[0] && a_fPass) ||
					(a_fRes != fVal[0] && !a_fPass)) ? 0 : 1;
				if (iRet == 1)
				{
					mu::console() << _T("\n  fail: ") << a_str.c_str()
						<< _T(" (incorrect result; expected: ") << a_fRes
						<< _T(" ;calculated: ") << fVal[0] << _T(").");
				}
			}
			catch (Parser::exception_type& e)
			{
				if (a_fPass)
				{
					mu::console() << _T("\n  fail: ") << e.GetExpr() << _T(" : ") << e.GetMsg();
					iRet = 1;
				}
			}
			catch (...)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (unexpected exception)");
				iRet = 1;  // exceptions other than ParserException are not allowed
			}

			return iRet;
		}

		//---------------------------------------------------------------------------
		/** \brief Test an expression in Bulk Mode. */
		int ParserTester::EqnTestBulk(const string_type& a_str, double a_fRes[4], bool a_fPass)
		{
			ParserTester::c_iCount++;

			// Define Bulk Variables
			int nBulkSize = 4;
			value_type vVariableA[] = { 1, 2, 3, 4 };   // variable values
			value_type vVariableB[] = { 2, 2, 2, 2 };   // variable values
			value_type vVariableC[] = { 3, 3, 3, 3 };   // variable values
			value_type vResults[] = { 0, 0, 0, 0 };   // variable values
			int iRet(0);

			try
			{
				Parser p;
				p.DefineConst(_T("const1"), 1);
				p.DefineConst(_T("const2"), 2);
				p.DefineVar(_T("a"), vVariableA);
				p.DefineVar(_T("b"), vVariableB);
				p.DefineVar(_T("c"), vVariableC);

				p.SetExpr(a_str);
				p.Eval(vResults, nBulkSize);

				bool bCloseEnough(true);
				for (int i = 0; i < nBulkSize; ++i)
				{
					bCloseEnough &= (fabs(a_fRes[i] - vResults[i]) <= fabs(a_fRes[i] * 0.00001));
				}

				iRet = ((bCloseEnough && a_fPass) || (!bCloseEnough && !a_fPass)) ? 0 : 1;
				if (iRet == 1)
				{
					mu::console() << _T("\n  fail: ") << a_str.c_str()
						<< _T(" (incorrect result; expected: {") << a_fRes[0] << _T(",") << a_fRes[1] << _T(",") << a_fRes[2] << _T(",") << a_fRes[3] << _T("}")
						<< _T(" ;calculated: ") << vResults[0] << _T(",") << vResults[1] << _T(",") << vResults[2] << _T(",") << vResults[3] << _T("}");
				}
			}
			catch (Parser::exception_type& e)
			{
				if (a_fPass)
				{
					mu::console() << _T("\n  fail: ") << e.GetExpr() << _T(" : ") << e.GetMsg();
					iRet = 1;
				}
			}
			catch (...)
			{
				mu::console() << _T("\n  fail: ") << a_str.c_str() << _T(" (unexpected exception)");
				iRet = 1;  // exceptions other than ParserException are not allowed
			}

			return iRet;
		}

		//---------------------------------------------------------------------------
		/** \brief Internal error in test class Test is going to be aborted. */
		void ParserTester::Abort() const
		{
			mu::console() << _T("Test failed (internal error in test class)") << endl;
			while (!getchar());
			exit(-1);
		}
	} // namespace test
} // namespace mu
