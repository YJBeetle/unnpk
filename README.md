# unnpk
解包网易neox游戏npk包

# build

```
git clone https://github.com/YJBeetle/unnpk
cd unnpk
make
```
## CentOS

```
sudo yum install file-libs file-devel
```

# run

```
./unnpk npk_file unnpk_direcory
```

# 如何解开阴阳师的script.npk

以下将会大致讲解如何解开阴阳师的script.npk
这里将会使用 MacOS ，其他建议使用 FreeBSD & Linux，相关命令也自行理解替换为你的操作系统使用的命令，不建议使用 Windows 太折腾了.
如果您有任何希望补充的欢迎pr

## 准备工作

按照前文所示编译好unnpk

安装Python2.7
```
brew install python2
```

pip安装以下需要的一些依赖
```
pip install rotor
```

安装uncompyle2
```
git clone https://github.com/wibiti/uncompyle2.git
cd uncompyle2
python setup.py install
```

## 开始解包

```
./unnpk script.npk script
```

取出一个解开后的文件，比如这里将会使用 0A0D60DC 这个文件作为案例

首先进行解密， tools/script_redirect.py 这个文件是根据通过逆向阴阳师得到的 redirect.py 改写而来

```
./tools/script_redirect.py 0A0D60DC > 0A0D60DC.out
```

纠正 opcode（这里的pyc_decryptor.py感谢fate0的文章： http://blog.fatezero.org/2017/01/14/decrypt-onmyoji/ ）

```
./tools/pyc_decryptor.py 0A0D60DC.out 0A0D60DC.pyc
```

反编译

```
uncompyle2 -o 0A0D60DC.py 0A0D60DC.pyc
```

至此反编译完毕

## 注意⚠️
这里只是演示了解开阴阳师的方法，至于网易其他的游戏，redirect.py中的asdf_dn、asdf_dt、asdf_df可能不同（也有可能相同）
这里顺便再说一下如何获取具体的redirect.py的方法吧，在 @fate0 的文章中，他使用动态调试的方法获取到了redirect.pyc，那么问题是redirect.pyc储存在哪里呢，事实上redirect.pyc就储存在script.npk里，以阴阳师2018-03-27更新的3.0.3(1)版本为例，redirect.pyc就是用本工具解开的FB54F059这个文件（文件名用npk中的偏移量命名），这里为各位解包其他游戏提供思路，具体是如何找到这个文件，方法很多，各位可自行去分析特征。

