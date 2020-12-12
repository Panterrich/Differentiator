all: main.cpp Differentiator.cpp libr/Tree.cpp libr/Str_lib.cpp libr/Stack.cpp libr/Guard.cpp
	g++ main.cpp Differentiator.cpp libr/Tree.cpp libr/Str_lib.cpp libr/Stack.cpp libr/Guard.cpp -o Differentiator
clean_all: 
	rm -f "images/equation.aux"
	rm -f "images/equation.log"
	rm -f "images/equation.out"
	rm -f "images/equation.pdf"
	rm -f "images/equation.tex"
	rm -f "images/end.log"
	rm -f "images/end.tex.bak"
	rm -f "images/equation.tex.bak"
	rm -f "images/preambule_article.tex.bak"
	rm -f "images/title.tex.bak"
	rm -f "images/title.log"
	rm -f "images/tree_graph.dot"
	rm -f images/base_*
clean:
	rm -f "images/equation.aux"
	rm -f "images/equation.log"
	rm -f "images/equation.out"
	rm -f "images/equation.tex"
	rm -f "images/tree_graph.dot"
	rm -f "images/end.log"
	rm -f "images/end.tex.bak"
	rm -f "images/equation.tex.bak"
	rm -f "images/preambule_article.tex.bak"
	rm -f "images/title.tex.bak"
	rm -f "images/title.log"

calc: calculator.cpp libr/Str_lib.cpp Differentiator.cpp Tree.cpp libr/Stack.cpp libr/Guard.cpp 
	g++ calculator.cpp libr/Str_lib.cpp Differentiator.cpp Tree.cpp libr/Stack.cpp libr/Guard.cpp -o calc

debug: 
	g++ -D NDEBUG -g -std=c++14 -Werror -fmax-errors=1 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++0x-compat 				\
	-Wc++11-compat -Wc++14-compat -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy 	\
	-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline 								\
	-Wlogical-op -Wmissing-declarations -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls	\
	-Wshadow -Wsign-conversion -Wsign-promo -Wstack-usage=8192 -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn 		\
	-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef 					\
	-Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing 		\
	-Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow 					\
	-flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds 			\
	-fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak 		\
	-fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute 				\
	-fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound 					\
	-fsanitize=vptr -lm -pie																												\
	g++ main.cpp Differentiator.cpp Tree.cpp libr/Str_lib.cpp libr/Stack.cpp libr/Guard.cpp -o Differentiator