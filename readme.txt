该update_postgressql.c在xubuntu18.04验证ok，需要安装libpqxxx*开发包


编译命令# gcc -I/usr/include/postgresql update_postgressql.c -L/usr/lib  -lpq  -o update_postgressql
