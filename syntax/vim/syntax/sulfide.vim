" Script:   sulfide.vim
" Homepage: http://github.com/qookei/sulfide
" Purpose:  Sulfide Syntax Highlighting

if version < 600
	syntax clear
elseif exists("b:current_syntax")
	finish
endif

syn case match

syn keyword sulKeyword let const fn if else while type value
syn match sulOperator "[+\-\*/\<\>@:=\.]"
syn match sulOperator "::"

syn keyword sulTodo TODO XXX FIXME HACK contained 
syn cluster sulCommentGrp contains=sulTodo
syn region sulComment start='#' end='\n' contains=@sulCommentGrp

syn match sulStringEscape /\\x[0-9a-fA-F_]\{2,}/
syn match sulStringEscape /\\b[01_]\{8,}/ 
syn match sulStringEscape /\\['"ntr]/ 
syn cluster sulStringGrp contains=sulStringEscape
syn region sulString start=/'/ skip=/\\./ end=/'/ contains=@sulStringGrp
syn region sulString start=/"/ skip=/\\./ end=/"/ contains=@sulStringGrp

syn match sulDecLiteral /\<[0-9]\+\>/
syn match sulHexLiteral /\<0x[0-9a-fA-F]\+\>/
syn match sulBinLiteral /\<0b[01]\+\>/
syn match sulOctLiteral /\<0o[0-7]\+\>/

syn region sulBlock start='{' end='}' fold transparent

hi def link sulComment      Comment
hi def link sulKeyword      Keyword
hi def link sulOperator     Operator
hi def link sulString       String
hi def link sulDecLiteral   sulLiteral
hi def link sulHexLiteral   sulLiteral
hi def link sulBinLiteral   sulLiteral
hi def link sulOctLiteral   sulLiteral
hi def link sulLiteral      Number
hi def link sulTodo         Todo
hi def link sulStringEscape SpecialChar

let b:current_syntax = "sulfide"
