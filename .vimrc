set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'

" The following are examples of different formats supported.
" Keep Plugin commands between vundle#begin/end.
" plugin on GitHub repo
Plugin 'tpope/vim-fugitive'
" The sparkup vim script is in a subdirectory of this repo called vim.
" Pass the path to set the runtimepath properly.
Plugin 'rstacruz/sparkup', {'rtp': 'vim/'}
" Install YCM
Plugin 'Valloric/YouCompleteMe'
" Install vim startify
Plugin 'mhinz/vim-startify'
" Install vim-windowswap
Plugin 'wesQ3/vim-windowswap'
" Install vim-autotag
Plugin 'craigemery/vim-autotag'
" Install tagbar
Plugin 'majutsushi/tagbar'
" Install ConqueGdb
Plugin 'vim-scripts/Conque-GDB'
" Install nerdtree
Plugin 'scrooloose/nerdtree'
" Install nerdtree git plugin
Plugin 'Xuyuanp/nerdtree-git-plugin'

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList       - lists configured plugins
" :PluginInstall    - installs plugins; append `!` to update or just :PluginUpdate
" :PluginSearch foo - searches for foo; append `!` to refresh local cache
" :PluginClean      - confirms removal of unused plugins; append `!` to auto-approve removal
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line
"set mouse+=r
set mouse=a
"if &term =~ '^screen'
" tmux knows the extended mouse mode
	set ttymouse=xterm2
"endif
noremap <C-W>M <C-W>\| <C-W>_
noremap <C-W>m <C-W>=
execute pathogen#infect()
syntax on


let mapleader=","
let g:windowswap_map_keys = 0 "prevent default bindings
nnoremap <silent> <leader>yw :call WindowSwap#MarkWindowSwap()<CR>
nnoremap <silent> <leader>pw :call WindowSwap#DoWindowSwap()<CR>
nnoremap <silent> <leader>ww :call WindowSwap#EasyWindowSwap()<CR>
noremap <F2> :AnsiEsc<CR>
noremap <F9> :so build_project.vim<CR>:%g/^{/normal! zf%<CR>gg<CR> 
noremap <F3> :so test_project.vim <CR>:%g/^{/normal! zf%<CR>gg<CR>
noremap <silent> <leader>fa :%g/^{/normal! zf%<CR>
noremap <silent> <leader>jf <C-]><CR>
noremap <silent> <leader>jb <C-t><CR>
noremap <silent> <leader>tb :TagbarToggle<CR>
noremap <silent> <leader>ft :NERDTreeToggle<CR>
noremap <silent> <leader>gb :Gblame<CR>
noremap <leader>xml :so xml_project_output.vim<CR>
noremap <leader>M <C-W>\| <C-W>_
noremap <leader>m <C-W>=
"noremap <F3> :echo 'Current time is ' . strftime('%c')<CR>
"nnoremap <silent> <F2> :lchdir %:p:h<CR>:pwd<CR>
set showcmd
set sessionoptions+=folds
set statusline+=_col:\ %c,
augroup XML
	autocmd!
	autocmd FileType xml setlocal foldmethod=indent foldlevelstart=999 foldminlines=0
augroup END
set autoread	
set term=screen-256color
colorscheme desert
set foldnestmax=1

if exists('$TMUX')
	autocmd BufEnter * call system("tmux rename-window " . expand("%:t"))
	autocmd VimLeave * call system("tmux rename-window bash")
      	autocmd BufEnter * let &titlestring = ' ' . expand("%:t")
	set title
endif
