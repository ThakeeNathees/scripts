
call plug#begin()

Plug 'https://github.com/vim-airline/vim-airline' " Status bar
Plug 'https://github.com/preservim/nerdtree' " NerdTree
Plug 'https://github.com/preservim/tagbar' " Tagbar for code navigation
Plug 'https://github.com/neoclide/coc.nvim'  " Auto Completion
Plug 'https://github.com/dstein64/nvim-scrollview' "Scroll bar
Plug 'https://github.com/tpope/vim-surround' " Surrounding ysw)
Plug 'https://github.com/tpope/vim-commentary' " For Commenting gcc & gc
Plug 'https://github.com/terryma/vim-multiple-cursors' " CTRL + N for multiple cursors
Plug 'https://github.com/rafi/awesome-vim-colorschemes' " Retro Scheme
Plug 'https://github.com/ryanoasis/vim-devicons' " Developer Icons
Plug 'https://github.com/ap/vim-css-color' " CSS Color Preview

call plug#end()

nnoremap <C-f> :NERDTreeFocus<CR>
nnoremap <C-l> :call CocActionAsync('jumpDefinition')<CR>
nmap <F8> :TagbarToggle<CR>

:set number
:set relativenumber
:set mouse=a

:set clipboard^=unnamed,unnamedplus " Share windows 10 clipboard with Vim.

:set tabstop=2     " Size of a hard tabstop (ts).
:set shiftwidth=2  " Size of an indentation (sw).
:set expandtab     " Always uses spaces instead of tab characters (et).
:set softtabstop=0 " Number of spaces a <Tab> counts for. When 0, featuer is off (sts).
:set autoindent    " Copy indent from current line when starting a new line.
:set smarttab      " Inserts blanks on a <Tab> key (as per sw, ts and sts).

:colorscheme jellybeans


