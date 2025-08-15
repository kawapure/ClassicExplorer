

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Mon Jan 18 20:14:07 2038
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


#ifndef __IBrandBand_FWD_DEFINED__
#define __IBrandBand_FWD_DEFINED__
typedef interface IBrandBand IBrandBand;

#endif 	/* __IBrandBand_FWD_DEFINED__ */


#ifndef __IBrowserHelperObject_FWD_DEFINED__
#define __IBrowserHelperObject_FWD_DEFINED__
typedef interface IBrowserHelperObject IBrowserHelperObject;

#endif 	/* __IBrowserHelperObject_FWD_DEFINED__ */


#ifndef __CAddressBarHostBand_FWD_DEFINED__
#define __CAddressBarHostBand_FWD_DEFINED__

#ifdef __cplusplus
typedef class CAddressBarHostBand CAddressBarHostBand;
#else
typedef struct CAddressBarHostBand CAddressBarHostBand;
#endif /* __cplusplus */

#endif 	/* __CAddressBarHostBand_FWD_DEFINED__ */


#ifndef __CBrandBand_FWD_DEFINED__
#define __CBrandBand_FWD_DEFINED__

#ifdef __cplusplus
typedef class CBrandBand CBrandBand;
#else
typedef struct CBrandBand CBrandBand;
#endif /* __cplusplus */

#endif 	/* __CBrandBand_FWD_DEFINED__ */


#ifndef __BrowserHelperObject_FWD_DEFINED__
#define __BrowserHelperObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class BrowserHelperObject BrowserHelperObject;
#else
typedef struct BrowserHelperObject BrowserHelperObject;
#endif /* __cplusplus */

#endif 	/* __BrowserHelperObject_FWD_DEFINED__ */


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


#ifndef __IBrandBand_INTERFACE_DEFINED__
#define __IBrandBand_INTERFACE_DEFINED__

/* interface IBrandBand */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBrandBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3E0BC6FD-A487-4438-B669-859D38F594A2")
    IBrandBand : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IBrandBandVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBrandBand * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBrandBand * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBrandBand * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBrandBand * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBrandBand * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBrandBand * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBrandBand * This,
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
    } IBrandBandVtbl;

    interface IBrandBand
    {
        CONST_VTBL struct IBrandBandVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBrandBand_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBrandBand_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBrandBand_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBrandBand_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBrandBand_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBrandBand_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBrandBand_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBrandBand_INTERFACE_DEFINED__ */


#ifndef __IBrowserHelperObject_INTERFACE_DEFINED__
#define __IBrowserHelperObject_INTERFACE_DEFINED__

/* interface IBrowserHelperObject */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBrowserHelperObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1D09EB49-083A-4679-A1D5-2F15CAD4D88F")
    IBrowserHelperObject : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IBrowserHelperObjectVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBrowserHelperObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBrowserHelperObject * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBrowserHelperObject * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBrowserHelperObject * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBrowserHelperObject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBrowserHelperObject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBrowserHelperObject * This,
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
    } IBrowserHelperObjectVtbl;

    interface IBrowserHelperObject
    {
        CONST_VTBL struct IBrowserHelperObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBrowserHelperObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBrowserHelperObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBrowserHelperObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBrowserHelperObject_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBrowserHelperObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBrowserHelperObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBrowserHelperObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBrowserHelperObject_INTERFACE_DEFINED__ */



#ifndef __AddressBarLib_LIBRARY_DEFINED__
#define __AddressBarLib_LIBRARY_DEFINED__

/* library AddressBarLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_AddressBarLib;

EXTERN_C const CLSID CLSID_CAddressBarHostBand;

#ifdef __cplusplus

class DECLSPEC_UUID("FC5A7312-CFFE-4BF1-83E8-22D44C040568")
CAddressBarHostBand;
#endif

EXTERN_C const CLSID CLSID_CBrandBand;

#ifdef __cplusplus

class DECLSPEC_UUID("9F7E1105-EB11-4A41-857A-F7E1494D3818")
CBrandBand;
#endif

EXTERN_C const CLSID CLSID_BrowserHelperObject;

#ifdef __cplusplus

class DECLSPEC_UUID("844CFA4E-7D74-4C80-B134-D214C9699F98")
BrowserHelperObject;
#endif
#endif /* __AddressBarLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


