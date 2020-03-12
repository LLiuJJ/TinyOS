# 学习过程中的错误记录
## 视频一
	1、make loader.o时报错：clang: error: unsupported option '--32'
		网上并没有关于"--32"的，但是有类似的，主要原因是应该用gcc make，但是我的电脑gcc实际是clang在make，修改成gcc make通过。
