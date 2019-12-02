该update_postgressql.c在xubuntu18.04验证ok，需要安装libpqxxx*开发包


编译命令# gcc -I/usr/include/postgresql update_postgressql.c -L/usr/lib  -lpq  -o update_postgressql

# upload projects or files into Github
# git init
# git add .
# git commit -m "first commit"
//if running #git remote have error, please exec # git pull --rebase origin maste 
# git remote add origin https://github.com/james0718/update_postgressql.git
# git pull --rebase origin master  //download README.md local computer
# git push -u origin master
