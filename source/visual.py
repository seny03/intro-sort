import streamlit as st
import pandas as pd


def main():
    uploaded_file = st.file_uploader("Загрузите файл с данными", type=["txt", "csv"])

    if uploaded_file is not None:
        try:
            df = pd.read_csv(uploaded_file, header=None, delimiter=' ')
            arr_sizes = df.iloc[0].dropna().values
            quick_random = df.iloc[1].dropna().values
            quick_reverse_sorted = df.iloc[2].dropna().values
            quick_almost_sorted = df.iloc[3].dropna().values
            intro_random = df.iloc[4].dropna().values
            intro_reverse_sorted = df.iloc[5].dropna().values
            intro_almost_sorted = df.iloc[6].dropna().values

            if len({len(arr_sizes), len(quick_random), len(quick_reverse_sorted)}) != 1:
                st.error("Ошибка: массивы должны быть одинаковой длины.")
                return

            data = pd.DataFrame({
                'Array size': arr_sizes,
                'Quick (Random)': quick_random,
                'Quick (Reversed)': quick_reverse_sorted,
                'Quick (Almost sorted)': quick_almost_sorted,
                'Intro (Random)': intro_random,
                'Intro (Reversed)': intro_reverse_sorted,
                'Intro (Almost sorted)': intro_almost_sorted
            }).set_index('Array size')

            st.line_chart(data, x_label='Array size', y_label='Execution time (ms)', color=["#E63946", "#F4A261", "#2A9D8F", "#A663CC", "#FFD166", "#4CC9F0"],
                          )

        except Exception as e:
            st.error(f"Ошибка при обработке файла: {e}")


if __name__ == "__main__":
    main()
