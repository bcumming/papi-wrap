program saxpy

    !use m_papi_wrap

    implicit none

    interface
        !---------------------------------------------------------------------------
        subroutine pw_new_collector_fortran(label, labellength, handle) &
            bind (c, name='pw_new_collector_fortran')
                use, intrinsic :: iso_c_binding
                character(c_char), intent(in)  :: label
                integer(c_int), value, intent(in)   :: labellength
                integer(c_int), intent(out)         :: handle
        end subroutine pw_new_collector_fortran

        !---------------------------------------------------------------------------
        subroutine pw_start_collector(handle) &
            bind (c, name='pw_start_collector')
                use, intrinsic :: iso_c_binding
                integer(c_int), value, intent(in)     :: handle
        end subroutine pw_start_collector

        !---------------------------------------------------------------------------
        subroutine pw_stop_collector(handle) &
            bind (c, name='pw_stop_collector')
                use, intrinsic :: iso_c_binding
                integer(c_int), value, intent(in)     :: handle
        end subroutine pw_stop_collector

        !---------------------------------------------------------------------------
        subroutine pw_print() &
            bind (c, name='pw_print')
                use, intrinsic :: iso_c_binding
        end subroutine pw_print
    end interface

    ! variables
    integer                    :: i, n
    integer                    :: handle1, handle2
    integer                    :: ierr
    real (kind=8), allocatable :: x(:), y(:)
    real (kind=8)              :: alpha

    ! initialize counters
    call pw_new_collector('init', handle1)
    call pw_new_collector('saxpy', handle2)
    print *, 'handles with values', handle1, 'and', handle2

    ! initialize data fields
    alpha = 0.5
    n = 100000000
    allocate(x(n), y(n), stat=ierr)

    ! set initial values in loop
    call pw_start_collector(handle1)
    !$omp parallel do
    do i=1,n
        x(i) = 1.0
        y(i) = i
    enddo
    !omp end parallel do
    call pw_stop_collector(handle1)

    ! perform saxpy
    call pw_start_collector(handle2)
    !$omp parallel do
    do i=1,n
        y(i) = alpha * x(i) + y(i)
    enddo
    !omp end parallel do
    call pw_stop_collector(handle2)

    print *, 'the saxpy loop took', 2*n, 'floating point operations', y(n)

    ! print counters to screen
    call pw_print()

contains

    subroutine pw_new_collector(label, handle)
        use, intrinsic :: iso_c_binding
        implicit none

        character, intent(in)         :: label*(*)
        integer(c_int), intent(out)   :: handle

        call pw_new_collector_fortran(label, len(label), handle)
    end subroutine pw_new_collector

end program saxpy

