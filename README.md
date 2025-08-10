# wsjcpp-sql-builder

C++ Class-helper for build SQL queries

## How to integrate to your project

via wsjcpp:
```
$ wsjcpp install https://github.com/wsjcpp/wsjcpp-sql-builder:master
```

or include this files:

- src/wsjcpp_sql_builder.h
- src/wsjcpp_sql_builder.cpp


## How use this

Example main func:
```
#include <wsjcpp_sql_builder.h>

int main(int argc, const char* argv[]) {
    // init employees
    bool bSilent = false;
    WsjcppEmployeesInit empls({}, bSilent);
    if (!empls.inited) {
        WsjcppLog::err(TAG, "Could not init employees");
        return -1;
    }
    return 0;
}
```

Example output:
```
% ./wsjcpp-employees
2020-03-22 11:32:31.750, 0x0x110c21dc0 [INFO] WJSCppEmployRuntimeGlobalCache: init
2020-03-22 11:32:31.750, 0x0x110c21dc0 [OK] Employees_init: Init WJSCppEmployRuntimeGlobalCache ... OK
```