!-------------------------------------------------------------------------------
! Fortran wrapper module for the papi wrapper
! the papi wrapper routines are implemented in C++, and exported via a C
! interface
!-------------------------------------------------------------------------------
module m_papi_wrap

use iso_c_binding

interface
    !---------------------------------------------------------------------------
    subroutine pw_new_collector(label, labellength, handle) &
        bind (c, name='pw_new_collector_fortran')
            use, intrinsic :: iso_c_binding
            integer(c_char), intent(in)          :: label
            integer(c_int), value, intent(in)    :: labellength
            integer(c_int), intent(out)   :: handle
    end subroutine pw_new_collector

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

end module m_papi_wrap

