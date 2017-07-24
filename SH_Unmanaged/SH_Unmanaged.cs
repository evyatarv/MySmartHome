using System;

using System.Runtime.InteropServices;

namespace SH_UnmanagedWrapper
{
    public static class SH_Unmanaged
    {
        const string SH_UNMANAGED_DLL = "SmartHomeManagedWrapper.dll";

        [DllImport(SH_UNMANAGED_DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateSwitch(
            byte num_of_relayes, 
            string address,
            Int32 address_len);

        [DllImport(SH_UNMANAGED_DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateContext(
            UInt32 transport_type,
            Int32 timeout,
            string auth_name,
            Int32 auth_name_len,
		    string auth_pass,
            Int32 auth_pass_len);

        [DllImport(SH_UNMANAGED_DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void On(
            IntPtr context,
            IntPtr sh_switch,
            UInt32 relat_index);

        [DllImport(SH_UNMANAGED_DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Off(
            IntPtr context,
            IntPtr sh_switch,
            UInt32 relat_index);

        [DllImport(SH_UNMANAGED_DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Alternate(
            IntPtr context,
            IntPtr sh_switch);
    }
}
