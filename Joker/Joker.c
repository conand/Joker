#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


PFLT_FILTER gFilterHandle;
ULONG_PTR OperationStatusCtx = 1;

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002

ULONG gTraceFlags = PTDBG_TRACE_ROUTINES | PTDBG_TRACE_OPERATION_STATUS;


#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(gTraceFlags,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))

/* Prototypes */

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    );

NTSTATUS
JokerInstanceSetup (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
    );

VOID
JokerInstanceTeardownStart (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    );

VOID
JokerInstanceTeardownComplete (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    );

NTSTATUS
JokerUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
    );

NTSTATUS
JokerInstanceQueryTeardown (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
JokerPreOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
    );

VOID
JokerOperationStatusCallback (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
    _In_ NTSTATUS OperationStatus,
    _In_ PVOID RequesterContext
    );

FLT_POSTOP_CALLBACK_STATUS
JokerPostOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
JokerPreOperationNoPostOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
    );

//  Assign text sections for each routine.

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, JokerUnload)
#pragma alloc_text(PAGE, JokerInstanceQueryTeardown)
#pragma alloc_text(PAGE, JokerInstanceSetup)
#pragma alloc_text(PAGE, JokerInstanceTeardownStart)
#pragma alloc_text(PAGE, JokerInstanceTeardownComplete)
#endif

//  operation registration

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {

    { IRP_MJ_CREATE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_CREATE_NAMED_PIPE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_CLOSE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_READ,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_WRITE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_QUERY_INFORMATION,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_SET_INFORMATION,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_QUERY_EA,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_SET_EA,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_FLUSH_BUFFERS,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_QUERY_VOLUME_INFORMATION,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_SET_VOLUME_INFORMATION,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_DIRECTORY_CONTROL,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_FILE_SYSTEM_CONTROL,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_DEVICE_CONTROL,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_INTERNAL_DEVICE_CONTROL,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_SHUTDOWN,
      0,
      JokerPreOperationNoPostOperation,
      NULL },                               //post operations not supported

    { IRP_MJ_LOCK_CONTROL,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_CLEANUP,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_CREATE_MAILSLOT,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_QUERY_SECURITY,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_SET_SECURITY,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_QUERY_QUOTA,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_SET_QUOTA,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_PNP,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_RELEASE_FOR_MOD_WRITE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_RELEASE_FOR_CC_FLUSH,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_NETWORK_QUERY_OPEN,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_MDL_READ,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_MDL_READ_COMPLETE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_PREPARE_MDL_WRITE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_MDL_WRITE_COMPLETE,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_VOLUME_MOUNT,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_VOLUME_DISMOUNT,
      0,
      JokerPreOperation,
      JokerPostOperation },

    { IRP_MJ_OPERATION_END }
};

//  This defines what we want to filter with FltMgr

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof( FLT_REGISTRATION ),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    NULL,                               //  Context
    Callbacks,                          //  Operation callbacks

    JokerUnload,                           //  MiniFilterUnload

    JokerInstanceSetup,                    //  InstanceSetup
    JokerInstanceQueryTeardown,            //  InstanceQueryTeardown
    JokerInstanceTeardownStart,            //  InstanceTeardownStart
    JokerInstanceTeardownComplete,         //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent

};


NTSTATUS
JokerInstanceSetup (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeDeviceType );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("Joker!JokerInstanceSetup: Entered\n") );

    return STATUS_SUCCESS;
}


NTSTATUS
JokerInstanceQueryTeardown (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    )
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("Joker!JokerInstanceQueryTeardown: Entered\n") );

    return STATUS_SUCCESS;
}


VOID
JokerInstanceTeardownStart (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    )
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("Joker!JokerInstanceTeardownStart: Entered\n") );
}


VOID
JokerInstanceTeardownComplete (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    )
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("Joker!JokerInstanceTeardownComplete: Entered\n") );
}


NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER( RegistryPath );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("Joker!DriverEntry: Entered\n") );

    //  Register with FltMgr
    status = FltRegisterFilter( DriverObject,
                                &FilterRegistration,
                                &gFilterHandle );

    FLT_ASSERT( NT_SUCCESS( status ) );

    if (NT_SUCCESS( status )) {

        status = FltStartFiltering( gFilterHandle );

        if (!NT_SUCCESS( status )) {

            FltUnregisterFilter( gFilterHandle );
        }
    }

    return status;
}

NTSTATUS
JokerUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
    )
{
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("Joker!JokerUnload: Entered\n") );

    FltUnregisterFilter( gFilterHandle );

    return STATUS_SUCCESS;
}


/* MiniFilter callback routines */
FLT_PREOP_CALLBACK_STATUS
JokerPreOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
    )
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
              ("Joker!JokerPreOperation: Entered\n") );

    PFLT_FILE_NAME_INFORMATION fileNameInfos;
    UNICODE_STRING fullPath, fileExtension, targetFile, tmpExtension;

    status = FltGetFileNameInformation(Data,
      FLT_FILE_NAME_NORMALIZED,
      &fileNameInfos);

    if (status == STATUS_SUCCESS){
    
      RtlCreateUnicodeString(&targetFile, L"\\Device\\HarddiskVolume2\\Users\\user\\Desktop\\target.mp3");

      fullPath = fileNameInfos->Name;

      if (!RtlEqualUnicodeString(&targetFile, &fullPath, FALSE)){
   
        // Get file extension
        status = FltParseFileName(&fullPath, &fileExtension, NULL, NULL); 

        if (status == STATUS_SUCCESS){
  
          RtlCreateUnicodeString(&tmpExtension, L"mp3");

          if (RtlEqualUnicodeString(&tmpExtension, &fileExtension, FALSE)){
            PFILE_OBJECT FileObject;

            // Change Target File
            FileObject = Data->Iopb->TargetFileObject;
            
            FileObject->FileName = targetFile;
            Data->IoStatus.Information = IO_REPARSE;
            Data->IoStatus.Status = STATUS_REPARSE;
            Data->Iopb->TargetFileObject->RelatedFileObject = NULL;
            FltSetCallbackDataDirty(Data);

            return FLT_PREOP_COMPLETE;
          }
        }
      }
    }

    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}


VOID
JokerOperationStatusCallback (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
    _In_ NTSTATUS OperationStatus,
    _In_ PVOID RequesterContext
    )
{
    UNREFERENCED_PARAMETER( FltObjects );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("Joker!JokerOperationStatusCallback: Entered\n") );

    PT_DBG_PRINT( PTDBG_TRACE_OPERATION_STATUS,
                  ("Joker!JokerOperationStatusCallback: Status=%08x ctx=%p IrpMj=%02x.%02x \"%s\"\n",
                   OperationStatus,
                   RequesterContext,
                   ParameterSnapshot->MajorFunction,
                   ParameterSnapshot->MinorFunction,
                   FltGetIrpName(ParameterSnapshot->MajorFunction)) );
}


FLT_POSTOP_CALLBACK_STATUS
JokerPostOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
    )
{
    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );
    UNREFERENCED_PARAMETER( Flags );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("Joker!JokerPostOperation: Entered\n") );

    return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
JokerPreOperationNoPostOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
    )
{
    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("Joker!JokerPreOperationNoPostOperation: Entered\n") );

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}
