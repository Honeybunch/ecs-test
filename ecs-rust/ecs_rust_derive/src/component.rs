use proc_macro::TokenStream;
use quote::quote;
use syn;

pub fn impl_component_macro(ast: &syn::DeriveInput) -> TokenStream {
    let name = &ast.ident;
    let gen = quote! {
        impl Component for #name {
            fn component_macro() {
                println!("Component Macro! My name is {}!", stringify!(#name));
            }
        }
    };
    gen.into()
}
