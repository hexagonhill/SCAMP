project('scamp','cpp',default_options : ['default_library=static'])

eigen = dependency('eigen', required : false)
thread = dependency('threads')

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
  eigen,
  thread
]

cppArgs = ['-march=native']
libs = []

hasCuda = add_languages('cuda', required: false) and not get_option('cpu_only')

if not hasCuda
  message('Compiling against CPU')
  cc = meson.get_compiler('cpp')
  
  avx2test = '''    
    #include <immintrin.h>
    int main()
    {
      __m256i a, b, c;
      const int src[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
      int dst[8];
      a =  _mm256_loadu_si256( (__m256i*)src );
      b =  _mm256_loadu_si256( (__m256i*)src );
      c = _mm256_add_epi32( a, b );
      _mm256_storeu_si256( (__m256i*)dst, c );
      for( int i = 0; i < 8; i++ ){
        if( ( src[i] + src[i] ) != dst[i] ){
          return -1;
        }
      }
      return 0;
    }'''
  
  avx2 = cc.compiles(avx2test,args:'-mavx2')
  
  if avx2
    message('Enabled AVX2')
    cppArgs += ['-mavx2']
  endif

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
