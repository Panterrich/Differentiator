all: main.cpp Differentiator.cpp Tree.cpp libr/Str_lib.cpp libr/Stack.cpp libr/Guard.cpp
	g++ main.cpp Differentiator.cpp Tree.cpp libr/Str_lib.cpp libr/Stack.cpp libr/Guard.cpp -o Differentiator
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