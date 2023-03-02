mod component;

use component::*;
use proc_macro::TokenStream;
use syn;

#[proc_macro_derive(Component)]
pub fn component_derive_macro(input: TokenStream) -> TokenStream {
    let ast = syn::parse(input).unwrap();
    impl_component_macro(&ast)
}
