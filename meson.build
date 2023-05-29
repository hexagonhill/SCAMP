project('scamp','cpp',default_options : ['default_library=static'])

eigen = dependency('eigen', required : false)

if not eigen.found()
	eigen = dependency('eigen3')
endif


scampSrc = files(
'src/common/common.cpp',
'src/common/profile.cpp',
'src/common/scamp_args.cpp',
'src/common/scamp_interface.cpp',
'src/common/scamp_utils.cpp',
'src/core/SCAMP.cpp',
'src/core/cpu_kernel/baseline/dispatch_baseline.cpp',
'src/core/cpu_kernel/cpu_kernels.cpp',
'src/core/cpu_kernel/kernel_dispatcher.cpp',
'src/core/cpu_stats.cpp',
'src/core/kernel_common.cpp',
'src/core/qt_helper.cpp',
'src/core/tile.cpp'
)

scampInc = include_directories('src')


scampDep = [
  eigen
]

cppArgs = []
libs = []

hasCuda = add_languages('cuda', required: false)

if not hasCuda
  message('Compiling against CPU')

else
  message('Compiling against CUDA')
  
  cuda = import('unstable-cuda')
  nvcc = meson.get_compiler('cuda')
  
  cudaSrc = files(
    'src/core/kernel_gpu_utils.cu'
    ,'src/core/kernels.cu'
    ,'src/core/qt_kernels.cu'
  )

  cppArgs += ['-D_HAS_CUDA_','-mno-outline-atomics']

  cudaArgs = cuda.nvcc_arch_flags(nvcc,'All')
  cudaArgs += ['-D_HAS_CUDA_']

  cudaToolkitDep = dependency('CUDAToolkit', method : 'cmake', modules: ['CUDA::cufft','CUDA::cudart'] )
  scampDep += [cudaToolkitDep]  
  
  scampcudalib = library('scampcudalib'
    ,cudaSrc
    ,include_directories : scampInc
    ,cuda_args: cudaArgs
  )

  libs += [scampcudalib]
endif


scamplib = library(
  'scamplib'
  ,scampSrc
  ,include_directories : scampInc
  ,cpp_args: cppArgs
  ,link_with : libs
  ,dependencies : scampDep
)

gflags = dependency('gflags')
scampDep += gflags
libs += [scamplib]
exe_files = files('src/main.cpp')

SCAMP = executable(
  'SCAMP'
  ,exe_files
  ,include_directories : scampInc
  ,cpp_args: cppArgs
  ,link_with : libs
  ,dependencies : scampDep
)

scamp_dep = declare_dependency(include_directories: scampInc, link_with: scamplib)
