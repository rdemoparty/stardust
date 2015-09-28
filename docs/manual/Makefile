index.html: src/*.adoc Makefile
	# asciidoctor -a linkcss -a stylesheet=./stylesheets/maker.css src/index.adoc -o index.html
	asciidoctor src/index.adoc -o index.html
	rm -rf ../../tools/editor/web/manual
	mkdir -p ../../tools/editor/web/manual
	cp index.html ../../tools/editor/web/manual
	cp -r assets ../../tools/editor/web/manual
	cp -r images ../../tools/editor/web/manual
	cp coderay-asciidoctor.css ../../tools/editor/web/manual

