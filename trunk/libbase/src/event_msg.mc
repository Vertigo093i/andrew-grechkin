; /*
;   HEADER SECTION
;  */
SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
               Warning=0x2:STATUS_SEVERITY_WARNING
               Error=0x3:STATUS_SEVERITY_ERROR
              )
;
;
FacilityNames=(System=0x0:FACILITY_SYSTEM
               Runtime=0x2:FACILITY_RUNTUME
               Stubs=0x3:FACILITY_STUBS
               Io=0x4:FACILITY_IO_ERROR_CODE
              )
;
; /*
;    MESSAGE DEFINITION SECTION
;  */
;
; /* Categories */
;

MessageIdTypedef=WORD

MessageId=0x1
SymbolicName=EV_CATEGORY_1
Language=English
Category 1
.
MessageId=0x2
SymbolicName=EV_CATEGORY_2
Language=English
Category 2
.
MessageId=0x3
SymbolicName=EV_CATEGORY_3
Language=English
Category 3
.


; /* Messages */

MessageIdTypedef=DWORD

MessageId=0x100
Severity=Success
Facility=Runtime
SymbolicName=EV_MSG_SUCCESS
Language=English
String_EV_MSG_SUCCESS
.
MessageId=0x200
Severity=Informational
Facility=Runtime
SymbolicName=EV_MSG_INFO
Language=English
String_EV_MSG_INFO
.
MessageId=0x300
Severity=Warning
Facility=Runtime
SymbolicName=EV_MSG_WARNING
Language=English
String_EV_MSG_WARNING
.
MessageId=0x400
Severity=Error
Facility=Runtime
SymbolicName=EV_MSG_ERROR
Language=English
String_EV_MSG_ERROR
.


; /* Insert string parameters */

MessageId=1000
Severity=Success
Facility=System
SymbolicName=PARAM_1
Language=English
Parameter1
.
MessageId=1001
Severity=Informational
Facility=System
SymbolicName=PARAM_2
Language=English
Parameter2
.
MessageId=1002
Severity=Warning
Facility=System
SymbolicName=PARAM_3
Language=English
Parameter3
.
MessageId=1003
Severity=Error
Facility=System
SymbolicName=PARAM_4
Language=English
Parameter4
.
