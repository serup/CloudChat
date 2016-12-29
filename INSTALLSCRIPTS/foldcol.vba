" Vimball Archiver by Charles E. Campbell, Jr., Ph.D.
UseVimball
finish
plugin/foldcol.vim	[[[1
80
" foldcol: Fold Column function
" Author:	Charles E. Campbell
" Date:		Nov 17, 2013
" Version:	3g	ASTRO-ONLY
" Usage:
" 	Using visual-block mode, select a block (use ctrl-v).  Press \vfc
" 	This operation will fold the selected block away.
" 	Using normal mode, press \vfc.  This operation will remove all
" 	FoldCol-generated inline-folds.
"
"   Note: this plugin requires Vince Negri's conceal-ownsyntax patch
"         See http://groups.google.com/group/vim_dev/web/vim-patches, Patch#14
"
" 	"But if any of you lacks wisdom, let him ask of God, who gives to
" 	all liberally and without reproach; and it will be given to him."
" 	(James 1:5)
" =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
" GetLatestVimScripts: 1161 1 :AutoInstall: foldcol.vim

" ---------------------------------------------------------------------
" Load Once: {{{1
if &cp || exists("g:loaded_foldcol") || !has("conceal")
 finish
endif
let g:loaded_foldcol= "v3g"

" ---------------------------------------------------------------------
" Public Interface: {{{1
if !hasmapto("<Plug>VFoldCol","v")
 vmap <unique> <Leader>vfc <Plug>VFoldCol
endif
vmap <silent> <Plug>VFoldCol	:<c-u>call <SID>FoldCol(1)<cr>

if !hasmapto("<Plug>NFoldCol","n")
 nmap <unique> <Leader>vfc <Plug>NFoldCol
endif
nmap <silent> <Plug>NFoldCol	:call <SID>FoldCol(0)<cr>

com!		-range -nargs=0 -bang FoldCol	call s:FoldCol(<bang>1)
silent! com	-range -nargs=0 -bang FC		call s:FoldCol(<bang>1)

" ---------------------------------------------------------------------
"  FoldCol: use visual block mode (ctrl-v) to select a block to fold {{{1
fun! s:FoldCol(dofold)
"  call Dfunc("FoldCol(dofold=".a:dofold.")")
"  call Decho("firstline#".a:firstline." lastline#".a:lastline." <".line("'<")." >".line("'>"))

  if a:dofold
   " make a new fold
   if &cole == 0
	let &cole= 1
   endif

   " upper left corner
   let line_ul = line("'<") - 1
   let col_ul  = virtcol("'<")  - 1

   " lower right corner
   let line_lr = line("'>")
   let col_lr  = virtcol("'>")
   if &selection ==# 'exclusive'
	" need to subtract the display width of the character at the end of the selection
	if exists('*strdisplaywidth')
		let col_lr -= strdisplaywidth(matchstr(getline(line_lr), '\%' . col("'>") . 'c.'), strdisplaywidth(strpart(getline(line_lr), 0, col("'>") - 1)))
	else
		let col_lr -= 1
	endif
   endif
  
"   call Decho('syn region FoldCol start="\%>'.line_ul.'l\%>'.col_ul.'v" end="\%>'.line_lr.'l\|\%>'.col_lr.'v" conceal')
   exe 'syn region FoldCol start="\%>'.line_ul.'l\%>'.col_ul.'v" end="\%>'.line_lr.'l\|\%>'.col_lr.'v" conceal containedin=ALL'
  else
   " remove all folded columns
   syn clear FoldCol
  endif
"  call Dret("FoldCol")
endfun

" ---------------------------------------------------------------------
" vim: ts=4 fdm=marker
doc/foldcol.txt	[[[1
63
*foldcol.txt*	FoldCol	: Column Folding			2015 Mar 160

Author:    Charles E. Campbell  <drNchipO@ScampbellPfamilyA.bizM>
	   (remove NOSPAM from Campbell's email to use)
Copyright: (c) 2004-2015 by Charles E. Campbell		*foldcol-copyright*
           The VIM LICENSE applies to AsNeeded.vim and AsNeeded.txt
           (see |copyright|) except use "AsNeeded" instead of "Vim"
	   No warranty, express or implied.  Use At-Your-Own-Risk.


==============================================================================
1. Contents						*foldcol-contents*

	1. Contents................................: |foldcol-contents|
	2. FoldCol Manual..........................: |foldcol|
	3. FoldCol History.........................: |foldcol-history|


==============================================================================
2. FoldCol Manual					*foldcol*

INSTALLATION:

MAPS:
	<ctrl-v> [move] \vfc :	Select column to be folded using visual block
				mode.  The \vfc (actually <Leader>vfc) will
				then column-fold the designated text.

	\vfc                 :	In normal mode, the \vfc (again, actually
				<Leader>vfc) will remove the column folding.

COMMANDS:

	<ctrl-v> [move] :FC  :	Just like the \vfc map for visual mode

	:FC!                 :	Will remove all column folding.

	:FoldCol[!] [move]   : same as :FC  (:FC is only available contingent
	                       on there not being a :FC command already
			       defined; :FoldCol will override any
			       pre-existing command of that name0
			     

==============================================================================
3. FoldCol History					*foldcol-history*
	v3	Apr 09, 2010 * :FoldCol and :FC commands added
			       help page written and included
			     * Maps changed from <Leader>fc to <Leader>vfc
			       to reduce clashes with other plugins
			     * Added "containedin=ALL" to allow it to do
			       folding while inside other syntax regions.
			     * Note that it will still not fold keywords.
		Jul 23, 2010 * changed conc to |'cole'| to correspond to
			       vim 7.3
		Nov 17, 2013 * (Ingo Karkat) provided a patch allowing foldcol
			       to work with |'selection'|=exclusive.
	v2	Dec 21, 2004 * initial release
	v1	Epoch        * never released


==============================================================================
vim:tw=78:ts=8:ft=help:fdm=marker:

