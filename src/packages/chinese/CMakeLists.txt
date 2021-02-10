if(${PACKAGE_CHINESE})
    add_library(package_chinese STATIC
            "chinese.cc"
			"chinese.h"
            )
endif()
