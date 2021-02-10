option(PACKAGE_DATABASE "database package" ON)

if(${PACKAGE_DATABASE})
    add_library(package_database STATIC
            "database.cc"
            )
endif()
