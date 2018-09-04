
set(INCLUDE_TSBL
  include/tsbl/interpreter.hpp
  include/tsbl/lexer.hpp
  include/tsbl/utf8.hpp
)

set(INCLUDE_LIB
  ${INCLUDE_TSBL}
)

set(INCLUDE_TSBL ${INCLUDE_TSBL} PARENT_SCOPE)
set(INCLUDE_LIB ${INCLUDE_TSBL} PARENT_SCOPE)
