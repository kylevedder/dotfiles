#dotfiles

This is a repository for storing all of my system configurations files.

## hgfastbranch
This will display in the terminal prompt the name of the mercurial branch you are currently working in.

To use it, add the executable to your path and then add the following to your `.bashrc`:
```
PS1="${PS1::-3}$(hgfastbranch "⎇ ")\$ "
```
