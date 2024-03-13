
/* NOTE simon: linux atomic operation
https://gcc.gnu.org/onlinedocs/gcc-4.4.3/gcc/Atomic-Builtins.html
*/

/* NOTE simon: From nothings, for thread local storage.

#ifdef defined( _MSC_VER )
__declspec(thread)
#elif defined(__clang__) || defined(__GNUC__)
__thread
#elif __cplusplus >= 201103L
thread_local
#elif __STDC_VERSION_ >= 201112L
_Thread_local
#endif

*/

#if !defined( THREADING_H )

#if !defined( THREAD_QUEUE_MAX_ENTRY_COUNT )
#define THREAD_QUEUE_MAX_ENTRY_COUNT 32
#endif

#if !defined( THREAD_MAX_COUNT )
#define THREAD_MAX_COUNT 8
#endif

#include <stdint.h>

#if defined WINDOWS

/* From winnt.h */
long _InterlockedIncrement( long volatile *Addend );
long _InterlockedCompareExchange( long volatile *Destination, long Exchange, long Comperand );
__int64 _InterlockedCompareExchange64( __int64 volatile *Destination, __int64 Exchange, __int64 Comperand );
void* _InterlockedCompareExchangePointer( void volatile** Destination, void* Exchange, void* Comperand );

typedef HANDLE thread_semaphore_t;
typedef HANDLE thread_event_t;
typedef HANDLE thread_mutex_t;
typedef uint32_t thread_id_t;

void thread_semaphore_create( thread_semaphore_t* semaphore, int32_t initial_count, int32_t maximum_count ) {
    deref( semaphore ) = CreateSemaphoreEx( 0, ( long ) initial_count, ( long ) maximum_count, 0, 0, SEMAPHORE_ALL_ACCESS );
}

void thread_semaphore_destroy( thread_semaphore_t* semaphore ) {
    CloseHandle( deref( semaphore ) );
}

void thread_semaphore_signal( thread_semaphore_t* semaphore, int32_t count ) {
    ReleaseSemaphore( deref( semaphore ), count, 0 );
}

void thread_semaphore_wait( thread_semaphore_t* semaphore ) {
    WaitForSingleObjectEx( deref( semaphore ), INFINITE, 0 );
}

void thread_event_create( thread_event_t* event ) {
    deref( event ) = CreateEventW( 0, 0, 0, 0 );
}

void thread_event_destroy( thread_event_t* event ) {
    CloseHandle( deref( event ) );
}

void thread_event_signal( thread_event_t* event ) {
    SetEvent( deref( event ) );
}

void thread_event_wait( thread_event_t* event ) {
    WaitForSingleObjectEx( deref( event ), INFINITE, 0 );
}

void thread_mutex_create( thread_mutex_t* mutex ) {
    deref( mutex ) = CreateMutex( 0, 0, 0 );
}

void thread_mutex_destroy( thread_mutex_t* mutex ) {
    CloseHandle( deref( mutex ) );
}

void thread_mutex_release( thread_mutex_t* mutex ) {
    ReleaseMutex( deref( mutex ) );
}

void thread_mutex_wait( thread_mutex_t* mutex ) {
    WaitForSingleObjectEx( deref( mutex ), INFINITE, 0 );
}

int32_t atomic_increment_32( int32_t volatile* variable ) {
    int32_t result = _InterlockedIncrement( ( long volatile * ) variable );
    return result;
}

int32_t atomic_compare_exchange_32( int32_t volatile* variable, int32_t exchange, int32_t compare ) {
    int32_t result = _InterlockedCompareExchange( ( long volatile * ) variable, exchange, compare );
    return result;
}

uint64_t atomic_compare_exchange_64( uint64_t volatile* variable, uint64_t exchange, uint64_t compare ) {
    uint64_t result = _InterlockedCompareExchange64( cast( int64_t volatile*, variable ), cast( int64_t, exchange ), cast( int64_t, compare ) );
    return result;
}

void* atomic_compare_exchange_p( void volatile** variable, void* exchange, void* compare ) {
    void* result = _InterlockedCompareExchangePointer( variable, exchange, compare );
    return result;
}

void atomic_thread_fence( void ) {
    _mm_mfence( );
    // _WriteBarrier( );
}

void thread_set_name( data_t name ) {
    
    memory_get_on_stack( name_utf16, kibioctet( 4 ) );
    
    utf8_to_utf16( name, &name_utf16, &g_ucs_error );
    _assert( no_error( &g_ucs_error ) );
    string_zero_terminate_utf16( name_utf16 );
    
    SetThreadDescription( GetCurrentThread( ), cast( u16*, name_utf16.bytes ) );
}

#elif defined LINUX

#include <semaphore.h>
#include <pthread.h>

typedef sem_t thread_semaphore_t;
typedef sem_t thread_event_t;
typedef pthread_t thread_id_t;

void thread_semaphore_create( thread_semaphore_t* semaphore, int32_t initial_count, int32_t maximum_count ) {
    sem_init( semaphore, 0, initial_count );
}

void thread_semaphore_destroy( thread_semaphore_t* semaphore ) {
    sem_destroy( semaphore );
}

void thread_event_create( thread_event_t* event ) {
    sem_init( event, 0, 0 );
}

void thread_event_destroy( thread_event_t* event ) {
    sem_destroy( event );
}

void thread_event_signal( thread_event_t* event ) {
    int32_t value;
    sem_getvalue( event, &value );
    if ( value <= 0 ) {
        sem_post( event );
    }
}

void thread_event_wait( thread_event_t* event ) {
    sem_wait( event );
}

void thread_semaphore_signal( thread_semaphore_t* semaphore, int32_t count ) {
    for ( uintptr_t i = 0; i < count; i++ ) {
        sem_post( semaphore );
    }
}

void thread_semaphore_wait( thread_semaphore_t* semaphore ) {
    sem_wait( semaphore );
}

int32_t atomic_increment_32( int32_t volatile* variable ) {
    int32_t result = __atomic_add_fetch( variable, 1, __ATOMIC_SEQ_CST );
    return result;
}

int32_t atomic_compare_exchange_32( int32_t volatile* variable, int32_t exchange, int32_t compare ) {
    int32_t result = compare;
    __atomic_compare_exchange_n( variable, &result, exchange, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST );
    return result;
}

int64_t atomic_compare_exchange_64( int64_t volatile* variable, int64_t exchange, int32_t compare ) {
    /* NOTE simon (10/10/23 16:39:18): Not tested. */
    int64_t result = compare;
    __atomic_compare_exchange_n( variable, &result, exchange, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST );
    return result;
}

void* atomic_compare_exchange_64( void volatile** variable, void* exchange, void* compare ) {
    /* NOTE simon (10/10/23 16:39:27): Not tested. */
    void* result = compare;
    __atomic_compare_exchange_n( variable, &result, exchange, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST );
    return result;
}

void atomic_thread_fence( void ) {
    __atomic_thread_fence( __ATOMIC_SEQ_CST );
}

void thread_set_name( data_t name ) {
    /* int pthread_setname_np(pthread_t thread, const char *name); */
    /* Use the prctl(2) function with the option PR_SET_NAME (see the docs). */
    invalid_code_path( );
}

#else
# error Unsupported platform.
#endif

typedef void thread_callback_t( void* thread_callback_data );

typedef struct thread_queue_t thread_queue_t;

typedef struct thread_info_t {
    thread_queue_t* queue;
    uint32_t id;
    thread_id_t os_id;
    volatile long is_waiting;
    void* user_data;
} thread_info_t;

typedef struct thread_queue_entry_t {
    thread_callback_t* callback;
    void* callback_data;
} thread_queue_entry_t;

typedef struct thread_queue_t {
    
    thread_semaphore_t semaphore;
    thread_event_t complete_event;
    thread_event_t terminate_event;
    
    thread_queue_entry_t entries[ THREAD_QUEUE_MAX_ENTRY_COUNT ];
    thread_info_t threads[ THREAD_MAX_COUNT ];
    volatile int32_t next_entry_to_read;
    volatile int32_t next_entry_to_write;
    volatile int32_t completion_count;
    volatile int32_t completion_goal;
    volatile int32_t terminate;
    volatile int32_t terminated_count;
    uint32_t thread_count;
    
} thread_queue_t;

uint32_t thread_do_work( thread_queue_t* queue ) {
    
    profiler_event_start( thread_do_work );
    
    uint32_t result = 0;
    
    long next_entry = queue->next_entry_to_read;
    long next_entry_incremented = ( next_entry + 1 ) % THREAD_QUEUE_MAX_ENTRY_COUNT;
    
    if ( next_entry != queue->next_entry_to_write ) {
        
        result = 1;
        /* NOTE simon: Reading the entry needs to be done before the increment, otherwise if the
        planets align, it's possible that the main thread would overwrite the entry before it's read by
        the worker thread.
        https://hero.handmade.network/forums/code-discussion/t/3640-question_on___possible_problem_with_multithreading_queue */
        thread_queue_entry_t entry = queue->entries[ next_entry ];
        // long index = InterlockedCompareExchange( &queue->next_entry_to_read, next_entry_incremented, next_entry );
        long index = atomic_compare_exchange_32( &queue->next_entry_to_read, next_entry_incremented, next_entry );
        
        if ( index == next_entry ) {
            
            profiler_event_start( thread_user_function );
            entry.callback( entry.callback_data );
            profiler_event_end( thread_user_function );
            
            // if ( InterlockedIncrement( &queue->completion_count ) == queue->completion_goal ) {
            if ( atomic_increment_32( &queue->completion_count ) == queue->completion_goal ) {
                profiler_event_start( thread_queue_completed_signal );
                thread_event_signal( &queue->complete_event );
                profiler_event_end( thread_queue_completed_signal );
            }
        }
    }
    
    profiler_event_end( thread_do_work );
    
    return result;
}

#if defined WINDOWS
#define thread_return_type DWORD
#elif defined LINUX
#define thread_return_type void*
#else
# error Unsupported platform.
#endif

thread_return_type thread_main( void* thread_data ) {
    
    uint32_t error = 0;
    
    profiler_timeline_initialize( worker_thread, 1000, &error );
    
    thread_info_t* info = ( thread_info_t* ) thread_data;
    thread_queue_t* queue = info->queue;
    
    while ( !queue->terminate ) {
        
        profiler_collapsable_start( thread_loop );
        uint32_t work_left = thread_do_work( queue );
        
        if ( !work_left ) {
            info->is_waiting = 1;
            profiler_event_start( thread_wait_for_work );
            thread_semaphore_wait( &queue->semaphore );
            profiler_event_end( thread_wait_for_work );
            info->is_waiting = 0;
        }
        profiler_collapsable_end( thread_loop );
    }
    
    if ( ( uint32_t ) atomic_increment_32( &queue->terminated_count ) == queue->thread_count ) {
        /* NOTE simon: This event doesn't measure the terminate event because, when the signal
           is sent, the main thread might check the event pair before it's written by this thread. */
        profiler_event_start( thread_queue_terminated_signal );
        profiler_event_end( thread_queue_terminated_signal );
        thread_event_signal( &queue->terminate_event );
    }
    
    return 0;
}

void thread_initialize( thread_queue_t* queue, uintptr_t thread_count ) {
    
    if ( thread_count > THREAD_MAX_COUNT ) {
        thread_count = THREAD_MAX_COUNT;
    }
    
    queue->next_entry_to_read = 0;
    queue->next_entry_to_write = 0;
    queue->completion_count = 0;
    queue->completion_goal = 0;
    queue->terminate = 0;
    queue->terminated_count = 0;
    queue->thread_count = ( uint32_t ) thread_count;
    
    thread_semaphore_create( &queue->semaphore, 0, ( int32_t ) thread_count );
    thread_event_create( &queue->complete_event );
    thread_event_create( &queue->terminate_event );
    
    for ( uintptr_t i = 0; i < thread_count; i++ ) {
        thread_info_t* info = queue->threads + i;
        info->queue = queue;
        info->id = ( uint32_t ) i;
        info->is_waiting = 0;
        
#if defined WINDOWS
        HANDLE thread_handle = CreateThread( 0, 0, thread_main, info, 0, ( LPDWORD ) &info->os_id );
        SetThreadDescription( thread_handle, L"Worker thread" );
        CloseHandle( thread_handle );
#elif defined LINUX
        pthread_attr_t attribute;
        pthread_attr_init( &attribute );
        pthread_create( &info->os_id, &attribute, thread_main, info );
        pthread_attr_destroy( &attribute );
#else
# error Unsupported platform.
#endif
    }
}

uint32_t thread_queue_work( thread_queue_t* queue, thread_callback_t callback, void* callback_data ) {
    
    profiler_event_start( thread_queue_work );
    
    uint32_t result = 0;
    
    long next_entry_to_write = queue->next_entry_to_write;
    long next_entry_to_write_incremented = ( next_entry_to_write + 1 ) % THREAD_QUEUE_MAX_ENTRY_COUNT;
    
    if ( next_entry_to_write_incremented != queue->next_entry_to_read ) {
        
        thread_queue_entry_t* entry = queue->entries + next_entry_to_write;
        entry->callback = callback;
        entry->callback_data = callback_data;
        queue->completion_goal++;
        atomic_thread_fence( );
        queue->next_entry_to_write = next_entry_to_write_incremented;
        profiler_event_start( thread_queue_signal );
        thread_semaphore_signal( &queue->semaphore, 1 );
        profiler_event_end( thread_queue_signal );
        result = 1;
    }
    
    profiler_event_end( thread_queue_work );
    
    return result;
}

void thread_complete_queue( thread_queue_t* queue ) {
    
    profiler_event_start( thread_complete_queue );
    
    while ( queue->completion_count != queue->completion_goal ) {
        if ( !thread_do_work( queue ) ) {
            profiler_event_start( thread_wait_for_completion );
            thread_event_wait( &queue->complete_event );
            profiler_event_end( thread_wait_for_completion );
        }
    }
    
    queue->completion_count = 0;
    queue->completion_goal = 0;
    
    profiler_event_end( thread_complete_queue );
}

void thread_terminate( thread_queue_t* queue ) {
    
    profiler_event_start( thread_terminate );
    
    queue->terminated_count = 0;
    profiler_event_start( thread_terminate_signal );
    // InterlockedIncrement( &queue->terminate );
    atomic_increment_32( &queue->terminate );
    thread_semaphore_signal( &queue->semaphore, queue->thread_count );
    profiler_event_end( thread_terminate_signal );
    
    while ( ( uint32_t ) queue->terminated_count != queue->thread_count ) {
        profiler_event_start( thread_wait_for_termination );
        thread_event_wait( &queue->terminate_event );
        profiler_event_end( thread_wait_for_termination );
    }
    
    thread_semaphore_destroy( &queue->semaphore );
    
    profiler_event_end( thread_terminate );
}

#define THREADING_H
#endif

