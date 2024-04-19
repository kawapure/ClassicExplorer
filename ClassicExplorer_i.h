

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for ClassicExplorer.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ClassicExplorer_i_h__
#define __ClassicExplorer_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __IAddressBarHostBand_FWD_DEFINED__
#define __IAddressBarHostBand_FWD_DEFINED__
typedef interface IAddressBarHostBand IAddressBarHostBand;

#endif 	/* __IAddressBarHostBand_FWD_DEFINED__ */


#ifndef __IThrobberBand_FWD_DEFINED__
#define __IThrobberBand_FWD_DEFINED__
typedef interface IThrobberBand IThrobberBand;

#endif 	/* __IThrobberBand_FWD_DEFINED__ */


#ifndef __AddressBarHostBand_FWD_DEFINED__
#define __AddressBarHostBand_FWD_DEFINED__

#ifdef __cplusplus
typedef class AddressBarHostBand AddressBarHostBand;
#else
typedef struct AddressBarHostBand AddressBarHostBand;
#endif /* __cplusplus */

#endif 	/* __AddressBarHostBand_FWD_DEFINED__ */


#ifndef __ThrobberBand_FWD_DEFINED__
#define __ThrobberBand_FWD_DEFINED__

#ifdef __cplusplus
typedef class ThrobberBand ThrobberBand;
#else
typedef struct ThrobberBand ThrobberBand;
#endif /* __cplusplus */

#endif 	/* __ThrobberBand_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IAddressBarHostBand_INTERFACE_DEFINED__
#define __IAddressBarHostBand_INTERFACE_DEFINED__

/* interface IAddressBarHostBand */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IAddressBarHostBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("31371D7C-E550-4870-A3D2-6C3A1A553168")
    IAddressBarHostBand : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IAddressBarHostBandVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAddressBarHostBand * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAddressBarHostBand * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAddressBarHostBand * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAddressBarHostBand * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAddressBarHostBand * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAddressBarHostBand * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAddressBarHostBand * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IAddressBarHostBandVtbl;

    interface IAddressBarHostBand
    {
        CONST_VTBL struct IAddressBarHostBandVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAddressBarHostBand_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAddressBarHostBand_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAddressBarHostBand_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAddressBarHostBand_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAddressBarHostBand_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAddressBarHostBand_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAddressBarHostBand_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAddressBarHostBand_INTERFACE_DEFINED__ */


#ifndef __IThrobberBand_INTERFACE_DEFINED__
#define __IThrobberBand_INTERFACE_DEFINED__

/* interface IThrobberBand */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IThrobberBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3E0BC6FD-A487-4438-B669-859D38F594A2")
    IThrobberBand : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IThrobberBandVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IThrobberBand * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IThrobberBand * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IThrobberBand * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IThrobberBand * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IThrobberBand * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IThrobberBand * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IThrobberBand * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IThrobberBandVtbl;

    interface IThrobberBand
    {
        CONST_VTBL struct IThrobberBandVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IThrobberBand_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IThrobberBand_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IThrobberBand_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IThrobberBand_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IThrobberBand_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IThrobberBand_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IThrobberBand_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IThrobberBand_INTERFACE_DEFINED__ */



#ifndef __AddressBarLib_LIBRARY_DEFINED__
#define __AddressBarLib_LIBRARY_DEFINED__

/* library AddressBarLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_AddressBarLib;

EXTERN_C const CLSID CLSID_AddressBarHostBand;

#ifdef __cplusplus

class DECLSPEC_UUID("FC5A7312-CFFE-4BF1-83E8-22D44C040568")
AddressBarHostBand;
#endif

EXTERN_C const CLSID CLSID_ThrobberBand;

#ifdef __cplusplus

class DECLSPEC_UUID("9F7E1105-EB11-4A41-857A-F7E1494D3818")
ThrobberBand;
#endif
#endif /* __AddressBarLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


