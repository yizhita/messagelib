set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DMAIL_DATA_DIR="${CMAKE_SOURCE_DIR}/mimetreeparser/autotests/data" )
add_definitions( -DGRANTLEETHEME_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

set(test_libmessageviewer_SRCS)
ecm_qt_declare_logging_category(test_libmessageviewer_SRCS HEADER messageviewer_debug.h IDENTIFIER MESSAGEVIEWER_LOG CATEGORY_NAME org.kde.pim.messageviewer)

# convenience macro to add qtest unit tests
macro(add_messageviewer_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} setupenv.cpp util.cpp
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer KF5::Libkleo QGpgme Qt5::Test KF5::KIOCore KF5::Mime KF5::AkonadiCore KF5::CalendarCore KF5::WebEngineViewer
    )
endmacro ()

macro(add_messageviewer_class_unittest _source _additionalSource)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_additionalSource}
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer Qt5::Test KF5::IconThemes KF5::XmlGui KF5::I18n
    )
endmacro ()

add_messageviewer_unittest( messagedisplayformatattributetest.cpp )

# convenience macro to add qtest unit tests
macro(add_messageviewer_mailsourceviewbrowserwidget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ../src/widgets/mailsourceviewtextbrowserwidget.cpp ../src/findbar/findbarsourceview.cpp ${test_libmessageviewer_SRCS}
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES Qt5::Test Qt5::Gui Qt5::Widgets KF5::KIOCore KF5::Mime KF5::AkonadiCore KF5::CalendarCore KF5::PimTextEdit KF5::PimCommon KF5::MessageViewer KF5::WebEngineViewer KF5::SyntaxHighlighting KF5::I18n KF5::IconThemes
    )
endmacro ()


add_messageviewer_mailsourceviewbrowserwidget_unittest( mailsourceviewtextbrowserwidgettest.cpp )
########### viewertest_gui ###############

set(KDEPIMLIBS_RUN_ISOLATED_TESTS TRUE)
set(KDEPIMLIBS_RUN_SQLITE_ISOLATED_TESTS TRUE)

add_akonadi_isolated_test_advanced(viewertest.cpp "" "KF5::MessageViewer;KF5::XmlGui")
add_akonadi_isolated_test_advanced(viewergrantleethemesupporttest.cpp "" "KF5::MessageViewer;KF5::XmlGui")
add_akonadi_isolated_test_advanced(urlhandlermanagertest.cpp "util.cpp" "KF5::MessageViewer;KF5::Libkleo;QGpgme;KF5::KIOCore;KF5::Mime;KF5::AkonadiCore;KF5::CalendarCore;KF5::WebEngineViewer")
