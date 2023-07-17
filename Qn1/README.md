# Qn 1

## Details

The bash script and kernels in this folder are specified to be set up within `~/linux/CSC1107_assignment`.

## Usage

1. Compile kernel module and user binary

> Using Makefile
> ```shell
> make
> insmod CSC1107_7_kernel.ko
> ```

> Using shell script
> ```shell
> ./remake_ko.sh
> ```

2. Run user binary

```shell
./CSC1107_7_user
```

3. Remove kernel module and clean directory

```shell
rmmod CSC1107_7_kernel
make clean
```
