#!/bin/bash

astyle ./*.cpp,*.h,*.hpp \
	--recursive \
	--style=linux \
	--attach-namespaces \
	--attach-classes \
	--attach-closing-while \
	--suffix=none \
	--indent=force-tab=4 \
	--pad-oper \
	--pad-header \
	--unpad-paren \
	--align-pointer=type \
	--align-reference=type \
	--indent-col1-comments \
	--close-templates \
	--max-code-length=100 \
	--max-continuation-indent=100 \
	--break-after-logical \
	--convert-tabs \
	--indent-preproc-define
